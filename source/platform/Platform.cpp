#include "platform/Platform.hpp"

#include <iostream>
#include <vector>
#include <map>

#include "libvulkan-loader.hpp"
#include "framework/SemaphoreManager.hpp"
#include "framework/Context.hpp"

#ifdef FORCE_NO_VALIDATION
#define ENABLE_VALIDATION_LAYERS 0
#else
#define ENABLE_VALIDATION_LAYERS 1
#endif

namespace Tobi
{

//TODO: move to separate file
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
      surface(VK_NULL_HANDLE),
      physicalDevice(VK_NULL_HANDLE),
      queueFamilyProperties(std::vector<VkQueueFamilyProperties>()),
      supportedQueues(0),
      graphicsQueueFamilyIndex(-1),
      presentQueueFamilyIndex(-1),
      computeQueueFamilyIndex(-1),
      transferQueueFamilyIndex(-1),
      graphicsQueue(VK_NULL_HANDLE),
      presentQueue(VK_NULL_HANDLE),
      computeQueue(VK_NULL_HANDLE),
      transferQueue(VK_NULL_HANDLE),
      logicalDevice(VK_NULL_HANDLE),
      swapChain(VK_NULL_HANDLE),
      swapChainImages(std::vector<VkImage>()),
      semaphoreManager(nullptr),
      useInstanceExtensions(true),
      useDeviceExtensions(true),
      haveDebugReport(true),
      externalLayers(std::vector<std::string>()),
      activeInstanceExtensions(std::vector<const char *>()),
      activeInstanceLayers(std::vector<const char *>()),
      activeDeviceLayers(std::vector<const char *>()),
      debugReportCallback(VK_NULL_HANDLE),
      vsync(false)
{
    LOGI("CONSTRUCTING Platform\n");
}

Platform::~Platform()
{
    LOGI("DECONSTRUCTING Platform\n");
}

void Platform::terminate()
{
    LOGI("TERMINATING PlatformXcb\n");

    waitIdle();

    if (semaphoreManager)
    {
        semaphoreManager.reset(nullptr);
        semaphoreManager = nullptr;
    }

    destroySwapChain();

    if (logicalDevice)
    {
        vkDestroyDevice(logicalDevice, nullptr);
        logicalDevice = VK_NULL_HANDLE;
    }
    if (surface)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }
    if (debugReportCallback)
    {
        VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_EXTENSION_SYMBOL(instance, vkDestroyDebugReportCallbackEXT);
        vkDestroyDebugReportCallbackEXT(instance, debugReportCallback, nullptr);
        debugReportCallback = VK_NULL_HANDLE;
    }
    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}

void Platform::destroySwapChain()
{
    waitIdle();

    if (swapChain)
    {

        if (!vkDestroySwapchainKHR)
        {
            LOGE("Symbol vkDestroySwapchainKHR not loaded\n");
        }
        vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}

Result Platform::presentImage(uint32_t index, const VkSemaphore &releaseSemaphore)
{
    VkResult result;
    VkPresentInfoKHR present = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain;
    present.pImageIndices = &index;
    present.pResults = &result;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &releaseSemaphore;

    VkResult res = vkQueuePresentKHR(presentQueue, &present);

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
        return RESULT_ERROR_OUTDATED_SWAPCHAIN;
    else if (res != VK_SUCCESS)
        return RESULT_ERROR_GENERIC;
    else
        return RESULT_SUCCESS;
}

void Platform::waitIdle()
{
    if (logicalDevice)
    {
        vkDeviceWaitIdle(logicalDevice);
    }
}

Result Platform::acquireNextImage(uint32_t &swapChainIndex, VkSemaphore &acquireSemaphore)
{
    acquireSemaphore = semaphoreManager->getClearedSemaphore();
    VkResult res = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, &swapChainIndex);

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vkQueueWaitIdle(graphicsQueue);
        vkQueueWaitIdle(computeQueue);
        semaphoreManager->addClearedSemaphore(acquireSemaphore);

        // Recreate swapchain.
        if (SUCCEEDED(initSwapChain(swapChainDimensions)))
            return RESULT_ERROR_OUTDATED_SWAPCHAIN;
        else
            return RESULT_ERROR_GENERIC;
    }
    else if (res != VK_SUCCESS)
    {
        vkQueueWaitIdle(graphicsQueue);
        vkQueueWaitIdle(computeQueue);
        semaphoreManager->addClearedSemaphore(acquireSemaphore);
        return RESULT_ERROR_GENERIC;
    }

    return RESULT_SUCCESS;
}

void Platform::addClearedSemaphore(VkSemaphore clearedSemaphore)
{
    semaphoreManager->addClearedSemaphore(clearedSemaphore);
}

Result Platform::initVulkan(
    const SwapChainDimensions &swapChainDimensions,
    const std::vector<const char *> &requiredInstanceExtensions,
    const std::vector<const char *> &requiredDeviceExtensions)
{
    if (FAILED(initInstance(requiredInstanceExtensions)))
    {
        LOGE("Failed to init instance.");
        return RESULT_ERROR_GENERIC;
    }
    if (FAILED(initDebugReport()))
    {
        LOGE("Failed to init debug report.");
        return RESULT_ERROR_GENERIC;
    }
    if (FAILED(initPhysicalDevice(requiredDeviceExtensions)))
    {
        LOGE("Failed to init physical device.");
        return RESULT_ERROR_GENERIC;
    }
    if (FAILED(initSurface()))
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
    if (FAILED(initSwapChain(swapChainDimensions)))
    {
        LOGE("Failed to init swap chain.");
        return RESULT_ERROR_GENERIC;
    }

    semaphoreManager = std::make_unique<SemaphoreManager>(logicalDevice);

    return RESULT_SUCCESS;
}

// TODO: move to helper header ?
/// @brief Helper function to add external layers to a list of active ones.
/// @param activeInstanceLayers List of active layers to be used.
/// @param supportedLayers List of supported layers.
inline void addExternalLayers(const std::vector<std::string> &externalLayers,
                              std::vector<const char *> &activeInstanceLayers,
                              const std::vector<VkLayerProperties> &supportedLayers)
{
    for (auto &layer : externalLayers)
    {
        for (auto &supportedLayer : supportedLayers)
        {
            if (layer == supportedLayer.layerName)
            {
                activeInstanceLayers.push_back(supportedLayer.layerName);
                LOGI("Found external layer: %s\n", supportedLayer.layerName);
                break;
            }
        }
    }
}

// TODO: move to helper header ?
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

Result Platform::initInstance(
    const std::vector<const char *> &requiredInstanceExtensions)
{
    if (FAILED(initInstanceExtensions(requiredInstanceExtensions)))
    {
        LOGE("Failed to init Instance Extensions.");
        return RESULT_ERROR_GENERIC;
    }

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
        applicationCreateInfo.apiVersion = VK_API_VERSION_1_1;
        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        if (result == VK_SUCCESS)
            LOGI("Created Vulkan instance with API version 1.1.0.\n");
    }

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        applicationCreateInfo.apiVersion = VK_API_VERSION_1_0;
        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        if (result == VK_SUCCESS)
            LOGI("Created Vulkan instance with API version 1.0.0.\n");
    }

    if (result != VK_SUCCESS)
    {
        LOGE("Failed to create Vulkan instance (error: %d).\n", int(result));
        return RESULT_ERROR_GENERIC;
    }

    if (!vulkanSymbolWrapperLoadCoreInstanceSymbols(instance))
    {
        LOGE("Failed to load instance symbols.");
        return RESULT_ERROR_GENERIC;
    }

    if (FAILED(loadInstanceSymbols()))
    {
        LOGE("Failed to load instance symbols.");
        return RESULT_ERROR_GENERIC;
    }

    return RESULT_SUCCESS;
}

Result Platform::initDebugReport()
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

        if (!vkCreateDebugReportCallbackEXT)
        {
            LOGE("Symbol: vkCreateDebugReportCallbackEXT not loaded ");
            return RESULT_ERROR_GENERIC;
        }
        VK_CHECK(vkCreateDebugReportCallbackEXT(instance, &debufReportCallbackCreateInfo, nullptr, &debugReportCallback));
        LOGI("Enabling Vulkan debug reporting.\n");
    }
    return RESULT_SUCCESS;
}

Result Platform::initPhysicalDevice(
    const std::vector<const char *> &requiredDeviceExtensions)
{
    physicalDevice = pickPhysicalDevice();

    if (physicalDevice == VK_NULL_HANDLE)
    {
        LOGE("Failed to find a suitable deice for the application");
        abort();
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

    if (FAILED(initDeviceExtensions(requiredDeviceExtensions)))
    {
        LOGE("Failed to init device extensions.");
        return RESULT_ERROR_GENERIC;
    }

    return RESULT_SUCCESS;
}

int rateDeviceSuitability(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader)
    {
        return 0;
    }

    return score;
}

const VkPhysicalDevice Platform::pickPhysicalDevice() const
{
    uint32_t physicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

    if (physicalDeviceCount < 1)
    {
        LOGE("Failed to enumerate Vulkan physical device.\n");
        return VK_NULL_HANDLE;
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto &physicalDevice : physicalDevices)
    {
        int score = rateDeviceSuitability(physicalDevice);
        candidates.insert(std::make_pair(score, physicalDevice));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0)
    {
        return candidates.rbegin()->second;
    }
    else
    {
        return VK_NULL_HANDLE;
    }
}

/**
		* Get the index of a queue family that supports the requested queue flags
		*
		* @param queueFlags Queue flags to find a queue family index for
		*
		* @return Index of the queue family index that matches the flags
		*
		* @throw Throws an exception if no queue family index could be found that supports the requested flags
		*/
uint32_t Platform::getQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
    // Dedicated queue for compute
    // Try to find a queue family index that supports compute but not graphics
    if (queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            {
                supportedQueues |= VK_QUEUE_COMPUTE_BIT;
                return i;
                break;
            }
        }
    }

    // Dedicated queue for transfer
    // Try to find a queue family index that supports transfer but not graphics and compute
    if (queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
            {
                supportedQueues |= VK_QUEUE_TRANSFER_BIT;
                return i;
                break;
            }
        }
    }

    // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
    {
        if (queueFamilyProperties[i].queueFlags & queueFlags)
        {
            supportedQueues |= queueFlags;
            return i;
            break;
        }
    }
    LOGE("Could not find a matching queue family index.\n");
    return -1;
}

// TODO: specify which queue types that should be initialized
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

    graphicsQueueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
    computeQueueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
    transferQueueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);

    for (uint32_t i = 0; i < queueCount; i++)
    {
        // Check for presentation support.
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (queueFamilyProperties[i].queueCount > 0 && presentSupport)
        {
            presentQueueFamilyIndex = i;
            break;
        }
    }

    if (graphicsQueueFamilyIndex == -1)
    {
        LOGE("Did not find suitable queue which supports graphics.\n");
        return RESULT_ERROR_GENERIC;
    }

    return RESULT_SUCCESS;
}

Result Platform::initDevice(const std::vector<const char *> &requiredDeviceExtensions)
{
    // create device
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    float queuePriorities[] = {0.0f};

    if (supportedQueues & VK_QUEUE_GRAPHICS_BIT)
    {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        graphicsQueueCreateInfo.queueCount = 1;
        graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(graphicsQueueCreateInfo);
    }
    else
    {
        graphicsQueueFamilyIndex = VK_NULL_HANDLE;
    }
    if (supportedQueues & VK_QUEUE_COMPUTE_BIT && computeQueueFamilyIndex != graphicsQueueFamilyIndex)
    {
        VkDeviceQueueCreateInfo computeQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        computeQueueCreateInfo.queueFamilyIndex = computeQueueFamilyIndex;
        computeQueueCreateInfo.queueCount = 1;
        computeQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(computeQueueCreateInfo);
    }
    else
    {
        computeQueueFamilyIndex = graphicsQueueFamilyIndex;
    }
    if (supportedQueues & VK_QUEUE_TRANSFER_BIT && transferQueueFamilyIndex != graphicsQueueFamilyIndex && transferQueueFamilyIndex != computeQueueFamilyIndex)
    {
        VkDeviceQueueCreateInfo transferQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        transferQueueCreateInfo.queueFamilyIndex = transferQueueFamilyIndex;
        transferQueueCreateInfo.queueCount = 1;
        transferQueueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.emplace_back(transferQueueCreateInfo);
    }
    else
    {
        transferQueueFamilyIndex = graphicsQueueFamilyIndex;
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

    vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    if (graphicsQueueFamilyIndex == presentQueueFamilyIndex)
        presentQueue = graphicsQueue;
    else
        vkGetDeviceQueue(logicalDevice, presentQueueFamilyIndex, 0, &presentQueue);
    if (graphicsQueueFamilyIndex == computeQueueFamilyIndex)
        computeQueue = graphicsQueue;
    else
        vkGetDeviceQueue(logicalDevice, computeQueueFamilyIndex, 0, &computeQueue);
    if (graphicsQueueFamilyIndex == transferQueueFamilyIndex)
        transferQueue = graphicsQueue;
    else
        vkGetDeviceQueue(logicalDevice, transferQueueFamilyIndex, 0, &transferQueue);

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

    std::vector<VkPresentModeKHR> modes;
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);

    modes.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes.data());

    // FIFO is the only mode universally supported
    VkPresentModeKHR swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto m : modes)
    {
        if ((vsync && m == VK_PRESENT_MODE_MAILBOX_KHR) || (!vsync && m == VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            swapChainPresentMode = m;
            break;
        }
    }

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

    addExternalLayers(externalLayers, activeInstanceLayers, instanceLayers);
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
    addExternalLayers(externalLayers, activeDeviceLayers, deviceLayers);
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
