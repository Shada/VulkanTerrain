#include "platform/Platform.hpp"

#include <iostream>
#include <vector>

#include "libvulkan-loader.hpp"

#ifdef FORCE_NO_VALIDATION
#define ENABLE_VALIDATION_LAYERS 0
#else
#define ENABLE_VALIDATION_LAYERS 1
#endif

namespace Tobi
{

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
static const char *pValidationLayers[] = {
    "VK_LAYER_GOOGLE_threading",
    "VK_LAYER_LUNARG_parameter_validation",
    "VK_LAYER_LUNARG_object_tracker",
    "VK_LAYER_LUNARG_core_validation",
    "VK_LAYER_LUNARG_device_limits",
    "VK_LAYER_LUNARG_image",
    "VK_LAYER_LUNARG_swapchain",
    "VK_LAYER_GOOGLE_unique_objects",
};

static const char *pMetaLayers[] = {
    "VK_LAYER_LUNARG_standard_validation",
};

// TODO: move to another file
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
                                                    uint64_t object, size_t location, int32_t messageCode,
                                                    const char *pLayerPrefix, const char *pMessage, void *pUserData)
{
    auto *platform = static_cast<Platform *>(pUserData);
    auto callback = platform->getExternalDebugCallback();

    if (callback)
    {
        return callback(flags, type, object, location, messageCode, pLayerPrefix, pMessage,
                        platform->getExternalDebugCallbackUserData());
    }
    else
    {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            LOGE("Validation Layer: Error: %s: %s\n", pLayerPrefix, pMessage);
        }
        else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            LOGE("Validation Layer: Warning: %s: %s\n", pLayerPrefix, pMessage);
        }
        else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            LOGI("Validation Layer: Performance warning: %s: %s\n", pLayerPrefix, pMessage);
        }
        else
        {
            LOGI("Validation Layer: Information: %s: %s\n", pLayerPrefix, pMessage);
        }
        return VK_FALSE;
    }
}

Platform::Platform()
    : instance(VK_NULL_HANDLE),
      physicalDevice(VK_NULL_HANDLE),
      logicalDevice(VK_NULL_HANDLE),
      context(std::make_shared<Context>()),
      semaphoreManager(nullptr),
      externalLayers(std::vector<std::string>()),
      queueFamilyProperties(std::vector<VkQueueFamilyProperties>()),
      useInstanceExtensions(true),
      useDeviceExtensions(true),
      haveDebugReport(false),
      surface(VK_NULL_HANDLE),
      swapChain(VK_NULL_HANDLE),
      swapChainImages(std::vector<VkImage>()),
      activeInstanceExtensions(std::vector<const char *>()),
      activeInstanceLayers(std::vector<const char *>()),
      debugReportCallback(VK_NULL_HANDLE),
      externalDebugCallback(nullptr),
      pExternalDebugCallbackUserData(nullptr),
      graphicsQueue(VK_NULL_HANDLE),
      presentQueue(VK_NULL_HANDLE),
      computeQueue(VK_NULL_HANDLE),
      transferQueue(VK_NULL_HANDLE),
      graphicsQueueIndex(-1),
      presentQueueIndex(-1),
      computeQueueIndex(-1),
      transferQueueIndex(-1)
{
}

Platform::~Platform()
{
    terminate();
}

void Platform::terminate()
{
    // Don't release anything until the GPU is completely idle.
    if (logicalDevice)
        vkDeviceWaitIdle(logicalDevice);

    semaphoreManager->terminate();

    context->terminate();

    destroySwapChain();

    if (surface)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    if (logicalDevice)
    {
        vkDestroyDevice(logicalDevice, nullptr);
        logicalDevice = VK_NULL_HANDLE;
    }

    if (debugReportCallback)
    {
        VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkDestroyDebugReportCallbackEXT);
        vkDestroyDebugReportCallbackEXT(instance, debugReportCallback, nullptr);
        debugReportCallback = VK_NULL_HANDLE;
    }
    if (instance)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
        LOGI("Destroyed vulkan instance successfully\n");
    }

    vulkanSymbolWrapperUnload();
}

Result Platform::initVulkan(
    const SwapChainDimensions &swapChainDimensions,
    const std::vector<const char *> &requiredInstanceExtensions,
    const std::vector<const char *> &requiredDeviceExtensions)
{
    if (FAILED(initInstanceExtensions(requiredInstanceExtensions)))
    {
        LOGE("Failed to init Extensions.");
        return RESULT_ERROR_GENERIC;
    }

    if (FAILED(initInstance()))
    {
        LOGE("Failed to init instance.");
        return RESULT_ERROR_GENERIC;
    }

    if (!vulkanSymbolWrapperLoadCoreInstanceSymbols(instance))
    {
        LOGE("Failed to load instance symbols.");
        return RESULT_ERROR_GENERIC;
    }

    initDebugReport();

    if (FAILED(initPhysicalDevice()))
    {
        LOGE("Failed to init physical device.");
        return RESULT_ERROR_GENERIC;
    }

    if (FAILED(initDeviceExtensions(requiredDeviceExtensions)))
    {
        LOGE("Failed to init device extensions.");
        return RESULT_ERROR_GENERIC;
    }

    if (FAILED(loadInstanceSymbols()))
    {
        LOGE("Failed to load instance symbols.");
        return RESULT_ERROR_GENERIC;
    }

    surface = createSurface();
    if (surface == VK_NULL_HANDLE)
    {
        LOGE("Failed to init surface.");
        return RESULT_ERROR_GENERIC;
    }

    if (FAILED(initQueueFamilyIndices()))
    {
        LOGE("Failed to init queue indices.");
        return RESULT_ERROR_GENERIC;
    }
    if (FAILED(initDevice(requiredDeviceExtensions)))
    {
        LOGE("Failed to init device.");
        return RESULT_ERROR_GENERIC;
    }

    vkGetDeviceQueue(logicalDevice, graphicsQueueIndex, 0, &graphicsQueue);
    if (graphicsQueueIndex == presentQueueIndex)
        presentQueue = graphicsQueue;
    else
        vkGetDeviceQueue(logicalDevice, presentQueueIndex, 0, &presentQueue);
    if (graphicsQueueIndex == computeQueueIndex)
        computeQueue = graphicsQueue;
    else
        vkGetDeviceQueue(logicalDevice, computeQueueIndex, 0, &computeQueue);
    if (graphicsQueueIndex == transferQueueIndex)
        transferQueue = graphicsQueue;
    else
        vkGetDeviceQueue(logicalDevice, transferQueueIndex, 0, &transferQueue);

    auto result = initSwapChain(swapChainDimensions);
    if (result != RESULT_SUCCESS)
    {
        LOGE("Failed to init swapchain.");
        return result;
    }

    result = context->onPlatformUpdate(this);
    if (FAILED(result))
        return result;

    semaphoreManager = std::make_unique<SemaphoreManager>(logicalDevice);

    return RESULT_SUCCESS;
}

void Platform::destroySwapChain()
{
    if (logicalDevice)
        vkDeviceWaitIdle(logicalDevice);

    if (swapChain)
    {
        vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}

Result Platform::initDevice(
    const std::vector<const char *> &requiredDeviceExtensions)
{
    // create device
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    float queuePriorities[] = {0.0f};

    if (supportedQueues & VK_QUEUE_GRAPHICS_BIT)
    {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
        graphicsQueueCreateInfo.queueCount = 1;
        graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(graphicsQueueCreateInfo);
    }
    else
    {
        graphicsQueueIndex = VK_NULL_HANDLE;
    }
    if (supportedQueues & VK_QUEUE_COMPUTE_BIT && computeQueueIndex != graphicsQueueIndex)
    {
        VkDeviceQueueCreateInfo computeQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        computeQueueCreateInfo.queueFamilyIndex = computeQueueIndex;
        computeQueueCreateInfo.queueCount = 1;
        computeQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(computeQueueCreateInfo);
    }
    else
    {
        computeQueueIndex = graphicsQueueIndex;
    }
    if (supportedQueues & VK_QUEUE_TRANSFER_BIT && transferQueueIndex != graphicsQueueIndex && transferQueueIndex != computeQueueIndex)
    {
        VkDeviceQueueCreateInfo transferQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        transferQueueCreateInfo.queueFamilyIndex = transferQueueIndex;
        transferQueueCreateInfo.queueCount = 1;
        transferQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(transferQueueCreateInfo);
    }
    else
    {
        transferQueueIndex = graphicsQueueIndex;
    }

    VkPhysicalDeviceFeatures features = {false};
    VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    if (useDeviceExtensions)
    {
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
    }

#if ENABLE_VALIDATION_LAYERS
    if (!activeDeviceLayers.empty())
    {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(activeDeviceLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = activeDeviceLayers.data();
        LOGI("Using Vulkan device validation layers.\n");
    }
#endif

    deviceCreateInfo.pEnabledFeatures = &features;

    VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice));

    if (!vulkanSymbolWrapperLoadCoreDeviceSymbols(logicalDevice))
    {
        LOGE("Failed to load device symbols.");
        return RESULT_ERROR_GENERIC;
    }

    if (FAILED(loadDeviceSymbols()))
    {
        LOGE("Failed to load device symbols.");
        return RESULT_ERROR_GENERIC;
    }

    return RESULT_SUCCESS;
}

Result Platform::initInstance()
{
    VkApplicationInfo applicationCreateInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    applicationCreateInfo.pApplicationName = "Tobi Engine";
    applicationCreateInfo.applicationVersion = 0;
    applicationCreateInfo.pEngineName = "Tobi Engine";
    applicationCreateInfo.engineVersion = 0;
    applicationCreateInfo.apiVersion = VK_MAKE_VERSION(1, 1, 85);

    VkInstanceCreateInfo instanceCreateInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.pApplicationInfo = &applicationCreateInfo;
    if (useInstanceExtensions)
    {
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = activeInstanceExtensions.data();
    }

#if ENABLE_VALIDATION_LAYERS
    if (!activeInstanceLayers.empty())
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(activeInstanceLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = activeInstanceLayers.data();
        LOGI("Using Vulkan instance validation layers.\n");
    }
#endif
    // Create the Vulkan instance
    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

    // Try to fall back to compatible Vulkan versions if the driver is using
    // older, but compatible API versions.
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        applicationCreateInfo.apiVersion = VK_MAKE_VERSION(1, 0, 24);
        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        if (result == VK_SUCCESS)
            LOGI("Created Vulkan instance with API version 1.0.2.\n");
    }

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        applicationCreateInfo.apiVersion = VK_MAKE_VERSION(1, 0, 1);
        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        if (result == VK_SUCCESS)
            LOGI("Created Vulkan instance with API version 1.0.1.\n");
    }

    if (result != VK_SUCCESS)
    {
        LOGE("Failed to create Vulkan instance (error: %d).\n", int(result));
        return RESULT_ERROR_GENERIC;
    }

    return RESULT_SUCCESS;
}

void Platform::initDebugReport()
{
    if (haveDebugReport)
    {
        VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkCreateDebugReportCallbackEXT);
        VkDebugReportCallbackCreateInfoEXT debufReportCallbackCreateInfo = {VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT};
        debufReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                              VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                              VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

        debufReportCallbackCreateInfo.pfnCallback = debugCallback;
        debufReportCallbackCreateInfo.pUserData = this;

        if (vkCreateDebugReportCallbackEXT)
            vkCreateDebugReportCallbackEXT(instance, &debufReportCallbackCreateInfo, nullptr, &debugReportCallback);
        LOGI("Enabling Vulkan debug reporting.\n");
    }
}

Result Platform::initPhysicalDevice()
{
    uint32_t physicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

    if (physicalDeviceCount < 1)
    {
        LOGE("Failed to enumerate Vulkan physical device.\n");
        return RESULT_ERROR_GENERIC;
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

    for (const auto &physicalDevice : physicalDevices)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

#if defined(ARM) || defined(AARCH64)
        // If we are on an arm-device and have multiple GPUs in our system, try to find a Mali device.
        if (strstr(properties.deviceName, "Mali"))
        {
            physicalDevice = physicalDevice;
            LOGI("Found ARM Mali physical device: %s.\n", properties.deviceName);
            break;
        }
#else
// find the most suitable device here
#endif
    }

    // Fallback to the first GPU we find in the system.
    if (physicalDevice == VK_NULL_HANDLE)
        physicalDevice = physicalDevices.front();

    vkGetPhysicalDeviceProperties(physicalDevice, &gpuProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &gpuMemoryProperties);

    return RESULT_SUCCESS;
}

Result Platform::initQueueFamilyIndices()
{
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
    queueFamilyProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueFamilyProperties.data());
    if (queueCount < 1)
    {
        LOGE("Failed to query number of queues.");
        return RESULT_ERROR_GENERIC;
    }

    int32_t graphicsFamily = -1;
    int32_t presentFamily = -1;
    int32_t computeFamily = -1;
    int32_t transferFamily = -1;
    for (uint32_t i = 0; i < queueCount; i++)
    {
        // Check for graphics support.
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamily = i;
            graphicsQueueIndex = i;
            supportedQueues |= VK_QUEUE_GRAPHICS_BIT;
        }

        // Check for presentation support.
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (queueFamilyProperties[i].queueCount > 0 && presentSupport)
        {
            presentFamily = i;
            presentQueueIndex = i;
        }

        // Check for compute support.
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            computeFamily = i;
            computeQueueIndex = i;
            supportedQueues |= VK_QUEUE_COMPUTE_BIT;
        }

        // Check for transfer support.
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            transferFamily = i;
            transferQueueIndex = i;
            supportedQueues |= VK_QUEUE_TRANSFER_BIT;
        }

        if (graphicsFamily != -1 && presentFamily != -1 && computeFamily != -1 && transferFamily != -1)
        {
            break;
        }
    }

    if (graphicsFamily == -1)
    {
        LOGE("Did not find suitable queue which supports graphics, compute and "
             "presentation.\n");
        return RESULT_ERROR_GENERIC;
    }

    return RESULT_SUCCESS;
}

static void addSupportedLayers(
    std::vector<const char *> &activeInstanceLayers,
    const std::vector<VkLayerProperties> &instanceLayers,
    const char **ppRequestedLayers,
    uint32_t requestedCount)
{
    for (uint32_t i = 0; i < requestedCount; i++)
    {
        auto *requestedLayer = ppRequestedLayers[i];
        for (const auto &instanceLayer : instanceLayers)
        {
            if (std::strcmp(instanceLayer.layerName, requestedLayer) == 0)
            {
                activeInstanceLayers.push_back(requestedLayer);
                break;
            }
        }
    }
}

Result Platform::initInstanceExtensions(
    const std::vector<const char *> &requiredInstanceExtensions)
{
    if (!vulkanSymbolWrapperInitLoader())
    {
        LOGE("Cannot find Vulkan loader.\n");
        return RESULT_ERROR_GENERIC;
    }

    if (!vulkanSymbolWrapperLoadGlobalSymbols())
    {
        LOGE("Failed to load global Vulkan symbols.\n");
        return RESULT_ERROR_GENERIC;
    }

    uint32_t instanceExtensionCount;

    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr));
    std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data()));

    for (const auto &instanceExt : instanceExtensions)
        LOGI("Instance extension: %s\n", instanceExt.extensionName);

#if ENABLE_VALIDATION_LAYERS
    uint32_t instanceLayerCount;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));
    std::vector<VkLayerProperties> instanceLayers(instanceLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers.data()));

    // An instanceLayer could have VK_EXT_debug_report extension.
    for (const auto &instanceLayer : instanceLayers)
    {
        uint32_t count;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(instanceLayer.layerName, &count, nullptr));
        std::vector<VkExtensionProperties> extensions(count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(instanceLayer.layerName, &count, extensions.data()));
        for (const auto &extension : extensions)
            instanceExtensions.push_back(extension);
    }

    // On desktop, the LunarG loader exposes a meta-instanceLayer that combines all
    // relevant validation layers.
    addSupportedLayers(activeInstanceLayers, instanceLayers, pMetaLayers, NELEMS(pMetaLayers));

    // On Android, add all relevant layers one by one.
    if (activeInstanceLayers.empty())
    {
        addSupportedLayers(activeInstanceLayers, instanceLayers, pValidationLayers, NELEMS(pValidationLayers));
    }

    if (activeInstanceLayers.empty())
        LOGI("Did not find validation layers.\n");
    else
        LOGI("Found validation layers!\n");

    addExternalLayers(activeInstanceLayers, instanceLayers);
#endif

    useInstanceExtensions = true;
    if (!validateExtensions(requiredInstanceExtensions, instanceExtensions))
    {
        LOGI("Required instance extensions are missing, will try without.\n");
        useInstanceExtensions = false;
    }
    else
        activeInstanceExtensions = requiredInstanceExtensions;

    for (const auto &extension : instanceExtensions)
    {
        if (std::strcmp(extension.extensionName, "VK_EXT_debug_report") == 0)
        {
            haveDebugReport = true;
            useInstanceExtensions = true;
            activeInstanceExtensions.push_back("VK_EXT_debug_report");
            break;
        }
    }

    LOGI("Vulkan loaded extensions successfully\n");

    return RESULT_SUCCESS;
}

Result Platform::initDeviceExtensions(const std::vector<const char *> &requiredDeviceExtensions)
{
    uint32_t deviceExtensionCount;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr));
    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data()));

#if ENABLE_VALIDATION_LAYERS
    uint32_t deviceLayerCount;
    VK_CHECK(vkEnumerateDeviceLayerProperties(physicalDevice, &deviceLayerCount, nullptr));
    std::vector<VkLayerProperties> deviceLayers(deviceLayerCount);
    VK_CHECK(vkEnumerateDeviceLayerProperties(physicalDevice, &deviceLayerCount, deviceLayers.data()));

    activeDeviceLayers.clear();
    // On desktop, the LunarG loader exposes a meta-layer that combines all
    // relevant validation layers.
    addSupportedLayers(activeDeviceLayers, deviceLayers, pMetaLayers, NELEMS(pMetaLayers));

    // On Android, add all relevant layers one by one.
    if (activeDeviceLayers.empty())
    {
        addSupportedLayers(activeDeviceLayers, deviceLayers, pValidationLayers, NELEMS(pValidationLayers));
    }
    addExternalLayers(activeDeviceLayers, deviceLayers);
#endif

    for (const auto &deviceExtension : deviceExtensions)
        LOGI("Device extension: %s\n", deviceExtension.extensionName);

    useDeviceExtensions = true;
    if (!validateExtensions(requiredDeviceExtensions, deviceExtensions))
    {
        LOGI("Required device extensions are missing, will try without.\n");
        useDeviceExtensions = false;
    }

    return RESULT_SUCCESS;
}

Result Platform::initSwapChain(const SwapChainDimensions &dimensions)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

    VkSurfaceFormatKHR format;
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        format = surfaceFormats[0];
        format.format = dimensions.format;
    }
    else
    {
        if (formatCount == 0)
        {
            LOGE("Surface has no surfaceFormats.\n");
            return RESULT_ERROR_GENERIC;
        }

        format.format = VK_FORMAT_UNDEFINED;
        for (const auto &candidate : surfaceFormats)
        {
            switch (candidate.format)
            {
            // Favor UNORM surfaceFormats as the samples are not written for sRGB currently.
            case VK_FORMAT_R8G8B8A8_UNORM:
            case VK_FORMAT_B8G8R8A8_UNORM:
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
                format = candidate;
                break;

            default:
                break;
            }

            if (format.format != VK_FORMAT_UNDEFINED)
                break;
        }

        if (format.format == VK_FORMAT_UNDEFINED)
            format = surfaceFormats[0];
    }

    VkExtent2D swapChainSize;
    // -1u is a magic value (in Vulkan specification) which means there's no fixed
    // size.
    if (surfaceCapabilities.currentExtent.width == -1u)
    {
        swapChainSize.width = dimensions.width;
        swapChainSize.height = dimensions.height;
    }
    else
        swapChainSize = surfaceCapabilities.currentExtent;

    // FIFO must be supported by all implementations.
    VkPresentModeKHR swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Determine the number of VkImage's to use in the swapChain.
    // Ideally, we desire to own 1 image at a time, the rest of the images can
    // either be rendered to and/or
    // being queued up for display.
    uint32_t desiredSwapChainImages = surfaceCapabilities.minImageCount + 1;
    if ((surfaceCapabilities.maxImageCount > 0) && (desiredSwapChainImages > surfaceCapabilities.maxImageCount))
    {
        // Application must settle for fewer images than desired.
        desiredSwapChainImages = surfaceCapabilities.maxImageCount;
    }

    // Figure out a suitable surface transform.
    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        preTransform = surfaceCapabilities.currentTransform;

    VkSwapchainKHR oldSwapChain = swapChain;

    // Find a supported composite type.
    VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
        composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    else if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
        composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    else if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
        composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    else if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
        composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = desiredSwapChainImages;
    swapChainCreateInfo.imageFormat = format.format;
    swapChainCreateInfo.imageColorSpace = format.colorSpace;
    swapChainCreateInfo.imageExtent.width = swapChainSize.width;
    swapChainCreateInfo.imageExtent.height = swapChainSize.height;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.preTransform = preTransform;
    swapChainCreateInfo.compositeAlpha = composite;
    swapChainCreateInfo.presentMode = swapChainPresentMode;
    swapChainCreateInfo.clipped = true;
    swapChainCreateInfo.oldSwapchain = oldSwapChain;

    VK_CHECK(vkCreateSwapchainKHR(logicalDevice, &swapChainCreateInfo, nullptr, &swapChain));

    if (oldSwapChain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(logicalDevice, oldSwapChain, nullptr);

    swapChainDimensions.width = swapChainSize.width;
    swapChainDimensions.height = swapChainSize.height;
    swapChainDimensions.format = format.format;

    uint32_t imageCount;
    VK_CHECK(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr));
    swapChainImages.resize(imageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data()));

    return RESULT_SUCCESS;
}

bool Platform::validateExtensions(const std::vector<const char *> &requiredExtensions,
                                  const std::vector<VkExtensionProperties> &availableExtensions)
{
    for (const auto extension : requiredExtensions)
    {
        auto found = false;
        for (const auto &availableExtension : availableExtensions)
        {
            if (std::strcmp(availableExtension.extensionName, extension) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
            return false;
    }

    return true;
}

Result Platform::loadInstanceSymbols()
{
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkGetPhysicalDeviceSurfaceSupportKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkGetPhysicalDeviceSurfaceFormatsKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkGetPhysicalDeviceSurfacePresentModesKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkDestroySurfaceKHR))
        return RESULT_ERROR_GENERIC;
    return RESULT_SUCCESS;
}

Result Platform::loadDeviceSymbols()
{
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_EXTENSION_SYMBOL(logicalDevice, vkCreateSwapchainKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_EXTENSION_SYMBOL(logicalDevice, vkDestroySwapchainKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_EXTENSION_SYMBOL(logicalDevice, vkGetSwapchainImagesKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_EXTENSION_SYMBOL(logicalDevice, vkAcquireNextImageKHR))
        return RESULT_ERROR_GENERIC;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_EXTENSION_SYMBOL(logicalDevice, vkQueuePresentKHR))
        return RESULT_ERROR_GENERIC;
    return RESULT_SUCCESS;
}

} // namespace Tobi