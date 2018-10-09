#include <iostream>
#include <stdlib.h> 
#include <cassert>
#include <cstring>
#include <cstring>
#include <cstring>

#include "VulkanCore.hpp"
#include "util.hpp"

namespace Tobi
{
    VulkanCore::VulkanCore()
        : instance(nullptr),
          queueFamilyCount(0),
          surface(nullptr),
          device(nullptr),
          commandPool(nullptr),
          commandBuffer(nullptr),
          graphicsQueue(nullptr),
          presentQueue(nullptr),
          swapChain(nullptr),
          currentBuffer(0),
          swapChainImageCount(0),
          graphicsQueueFamilyIndex(0),
          presentQueueFamilyIndex(0),
          window(nullptr),
          depthBuffer{nullptr,nullptr,nullptr},
          depthBufferFormat(VK_FORMAT_UNDEFINED),
          uniformData{nullptr,nullptr,nullptr},
          pipelineLayout(nullptr),
          renderPass(nullptr)
    {
        WindowSettings settings;                                                               
        settings.width = 500;                                                                        
        settings.height = 300;                                                                       
                                                                                                     
        window = std::make_unique<WindowXcb>(settings);                                   
        window->createWindow();
        initVulkan();
    }
    VulkanCore::~VulkanCore()
    {
        vkDestroyPipeline(device, pipeline, nullptr);
        
        vkDestroyPipelineCache(device, pipelineCache, nullptr);

        vkDestroyDescriptorPool(device, descriptorPool, nullptr);

        vkDestroyBuffer(device, vertexBuffer.buffer, nullptr);
        vkFreeMemory(device, vertexBuffer.memory, nullptr);

        for (uint32_t i = 0; i < swapChainImageCount; i++)
        {
            vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
        }
        free(frameBuffers);
        if(shaderStages[0].module)
            vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
        if(shaderStages[1].module)
            vkDestroyShaderModule(device, shaderStages[1].module, nullptr);

        if(renderPass)
            vkDestroyRenderPass(device, renderPass, nullptr);
        if(pipelineLayout)
        {
            for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++) 
                vkDestroyDescriptorSetLayout(device, descriptorSetLayout[i], nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        }
        if(uniformData.buffer)
        {
            vkDestroyBuffer(device, uniformData.buffer, nullptr);
            vkFreeMemory(device, uniformData.memory, nullptr);
        }
        if(depthBuffer.view)
            vkDestroyImageView(device, depthBuffer.view, nullptr);
        if(depthBuffer.image)
            vkDestroyImage(device, depthBuffer.image, nullptr);
        if(depthBuffer.memory)
            vkFreeMemory(device, depthBuffer.memory, nullptr);
        if(swapChain)
        {
            for(uint32_t i = 0; i < swapChainImageCount; i++)
            {
                vkDestroyImageView(device, swapChainBuffers[i].view, nullptr);
            }
            vkDestroySwapchainKHR(device, swapChain, nullptr);
        }
        if(commandBuffer)
        {
            executeEndCommandBuffer();
            VkCommandBuffer commandBuffers[1] = {commandBuffer};
            vkFreeCommandBuffers(device, commandPool, 1, commandBuffers);
        }
        if(commandPool)
            vkDestroyCommandPool(device, commandPool, nullptr);
        if(device)
        {
            vkDeviceWaitIdle(device);
            vkDestroyDevice(device, nullptr);
        }

        if(surface)                                                                                  
            vkDestroySurfaceKHR(instance, surface, nullptr);     

        if(instance)
            vkDestroyInstance(instance, nullptr);
    }
static const char *vertexShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (std140, binding = 0) uniform bufferVals {\n"
    "    mat4 mvp;\n"
    "} myBufferVals;\n"
    "layout (location = 0) in vec4 pos;\n"
    "layout (location = 1) in vec4 inColor;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = inColor;\n"
    "   gl_Position = myBufferVals.mvp * pos;\n"
    "}\n";

static const char *fragmentShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (location = 0) in vec4 color;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = color;\n"
    "}\n";
struct Vertex {
    float posX, posY, posZ, posW;  // Position data
    float r, g, b, a;              // Color
};
#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_), 1.f
#define UV(_u_, _v_) (_u_), (_v_)
static const Vertex cubeData[] = {
    // red face
    {XYZ1(-1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
    // green face
    {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    // blue face
    {XYZ1(-1, 1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 1.f)},
    // yellow face
    {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 1.f, 0.f)},
    // magenta face
    {XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    // cyan face
    {XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
};

    void VulkanCore::initVulkan()
    {
        auto result = initGlobalLayerProperties();
        if(result != VK_SUCCESS)
        {
            std::cout << "Failed to init GlobalLayerProperties. " 
               << "Res: " << result << std::endl;
            return;
        }

        const bool depthPresent = true;

        initInstanceExtensionNames();
        initDeviceExtensionNames();
        initInstance("The funny app");
        initEnumerateDevice();
        initSwapchainExtension();
        initDevice();
        initCommandPool();
        initCommandBuffer();
        executeBeginCommandBuffer();
        initDeviceQueue();
        initSwapChain();
        initDepthBuffer();
        initUniformBuffer();
        initDescriptorAndPipelineLayouts(false);
        initRenderpass(depthPresent);
        initShaders(vertexShaderText, fragmentShaderText);
        initFrameBuffers(depthPresent);
        initVertexBuffer(cubeData, sizeof(cubeData), sizeof(cubeData[0]), false);
        initDescriptorPool(false);
        initDescriptorSet(false);
        initPipelineCache();
        initPipeline(depthPresent);
    }

    VkResult VulkanCore::initGlobalLayerProperties()
    {
        VkResult result = VK_SUCCESS;
        uint32_t instanceLayerCount;
        VkLayerProperties *vkLayerProperties = nullptr;
        do
        {
            result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
            if(result)
            {
                return result;
            }

            if(instanceLayerCount == 0)
            {
                return VK_SUCCESS;
            }

            vkLayerProperties = (VkLayerProperties*)realloc(
                    vkLayerProperties, 
                    instanceLayerCount * sizeof(VkLayerProperties));
            
            result = vkEnumerateInstanceLayerProperties(
                    &instanceLayerCount,
                    vkLayerProperties);
        }
        while(result == VK_INCOMPLETE);
        
        for(uint32_t i = 0; i < instanceLayerCount; i++)
        {
            LayerProperties layerProperties;
            layerProperties.properties = vkLayerProperties[i];
            result = initGlobalExtensionProperties(layerProperties);
            if(result)
                return result;
            instanceLayerProperties.push_back(layerProperties);
        }
        free(vkLayerProperties);

        return result;
    }

    VkResult VulkanCore::initGlobalExtensionProperties(LayerProperties &layerProperties)
    {
        VkExtensionProperties *instanceExtensions;
        uint32_t instanceExtensionCount;
        VkResult result = VK_SUCCESS;

        auto layerName = layerProperties.properties.layerName;
        
        do
        {
            result = vkEnumerateInstanceExtensionProperties(
                    layerName,
                    &instanceExtensionCount,
                    nullptr);
            
            if(result)
            {
                return result;
            }

            if(instanceExtensionCount == 0)
            {
                return VK_SUCCESS;
            }

            layerProperties.instanceExtensions.resize(instanceExtensionCount);
            instanceExtensions = layerProperties.instanceExtensions.data();
            result = vkEnumerateInstanceExtensionProperties(
                    layerName,
                    &instanceExtensionCount,
                    instanceExtensions);
        }
        while(result == VK_INCOMPLETE);

        return result;
    }

    void VulkanCore::initInstanceExtensionNames()
    {
        instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef __ANDROID__
        instanceExtensionNames.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
        instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
        instanceExtensionNames.push_back(VK_KHR_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND)
        instanceExtensionNames.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#else
        instanceExtensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
    }

    void VulkanCore::initDeviceExtensionNames()
    {
        deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    VkResult VulkanCore::initInstance(char const *const appShortName)
    {
        VkResult result = VK_SUCCESS;
        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pApplicationName = appShortName;
        applicationInfo.applicationVersion = 1;
        applicationInfo.pEngineName = appShortName;
        applicationInfo.engineVersion = 1;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayerNames.size());
        instanceCreateInfo.ppEnabledLayerNames = instanceLayerNames.size() 
            ? instanceLayerNames.data() 
            : nullptr;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames.data();

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        assert(result == VK_SUCCESS);

        return result;
    }
//TODO: move to some utility library
#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

    VkResult VulkanCore::initEnumerateDevice(uint32_t gpuCount)
    {
        uint32_t const U_ASSERT_ONLY requiredGpuCount = gpuCount;
        VkResult result = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
        assert(gpuCount);
        gpus.resize(gpuCount);

        result = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
        assert(!result && gpuCount >= requiredGpuCount);

        // TODO: is this something I should do for all gpus? if I have several, I want to utilize all
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, nullptr);
        assert(queueFamilyCount >= 1);

        queueProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
                gpus[0], 
                &queueFamilyCount, 
                queueProperties.data());
        assert(queueFamilyCount >= 1);

        vkGetPhysicalDeviceMemoryProperties(gpus[0], &memoryProperties);
        vkGetPhysicalDeviceProperties(gpus[0], &gpuProperties);
        
        for(auto &layerProperties : instanceLayerProperties)
        {
            initDeviceExtensionProperties(layerProperties);
        }

        return result;
    }

    VkResult VulkanCore::initDeviceExtensionProperties(LayerProperties &layerProperties)
    {
        VkResult result = VK_SUCCESS;
        
        auto layerName = layerProperties.properties.layerName;
        
        do
        {
            VkExtensionProperties *deviceExtensions;
            uint32_t deviceExtensionCount;

            result = vkEnumerateDeviceExtensionProperties(
                    gpus[0], 
                    layerName, 
                    &deviceExtensionCount,
                    nullptr);

            if(result)
            {
                return result;
            }

            if(deviceExtensionCount == 0)
            {
                return VK_SUCCESS;
            }

            layerProperties.deviceExtensions.resize(deviceExtensionCount);
            deviceExtensions = layerProperties.deviceExtensions.data();
            result = vkEnumerateDeviceExtensionProperties(
                    gpus[0], 
                    layerName,
                    &deviceExtensionCount,
                    deviceExtensions);                    
        }
        while(result == VK_INCOMPLETE);

        return result;
    }

    void VulkanCore::initSwapchainExtension()
    {
        VkResult U_ASSERT_ONLY result = VK_SUCCESS;

#ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.hinstance = window->getConnection();
        surfaceCreateInfo.hwnd = window->getWindow();
        result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(__ANDROID__)
        // this is not yet supported
        /*
        GET_INSTANCE_PROC_ADDR(instance, CreateAndroidSurfaceKHR);

        VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.window = AndroidGetApplicationWindow();
        result = fpCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
        */
#elif defined(VK_USE_PLATFOR_IOS_MVK)
        VkIOSSurfaceInfoMVK surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_IOS_CREATE_INFO_MVK;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.pView = window->getWindow();
        result = vkCreateIOSSurfaceMVK(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        VkMacOSSurfaceCreateInfo surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_STRUCTURE_CREATE_INFO_MVK;
        surfaceCreateInfo.pNext = nulptr;
        surfaceCreateInfo.flags - 0;
        surfaceCreateInfo.pView = window->getWindow();
#elif defined(VK_USE_PLATFORM_WAYLAND)
        VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.display = window->getDisplay();
        surfaceCreateInfo.surface = window->getWindow();
        result = vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#else
        VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.connection = window->getConnection();
        surfaceCreateInfo.window = window->getWindow();
        result = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#endif
        assert(result == VK_SUCCESS);
        
        VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
        for(uint32_t i = 0; i < queueFamilyCount; i++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(gpus[0], i, surface, &pSupportsPresent[i]);
        }

        graphicsQueueFamilyIndex = UINT32_MAX;
        presentQueueFamilyIndex = UINT32_MAX;
        for(uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            if((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                if(graphicsQueueFamilyIndex == UINT32_MAX) 
                    graphicsQueueFamilyIndex = i;

                if(pSupportsPresent[i] == VK_TRUE)
                {
                    graphicsQueueFamilyIndex = i;
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }

        if(presentQueueFamilyIndex == UINT32_MAX)
        {
            for(size_t i = 0; i < queueFamilyCount; ++i)
            {
                if(pSupportsPresent[i] == VK_TRUE)
                {
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }
        free(pSupportsPresent);
        

        if(graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX)
        {
            std::cout << "Could not find a queue for both graphics and present" << std::endl;
            exit(-1);
        }

        uint32_t formatCount;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, nullptr);
        assert(result == VK_SUCCESS);
        VkSurfaceFormatKHR *surfaceFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, surfaceFormats);
        assert(result == VK_SUCCESS);

        if(formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
        {
            format = VK_FORMAT_B8G8R8A8_UNORM;
        }
        else
        {
            assert(formatCount >= 1);
            format = surfaceFormats[0].format;
        }

        free(surfaceFormats);
    }

    VkResult VulkanCore::initDevice()
    {
        VkResult result = VK_SUCCESS;
        
        float queuePriorities[1] = {0.0};
        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.pNext = nullptr;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities;
        deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = nullptr;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceCreateInfo.enabledExtensionCount 
            ? deviceExtensionNames.data()
            : nullptr;
        deviceCreateInfo.pEnabledFeatures = nullptr;

        result = vkCreateDevice(gpus[0], &deviceCreateInfo, nullptr, &device);
        assert(result == VK_SUCCESS);

        return result;
    }

    void VulkanCore::initCommandPool()
    {
        VkResult U_ASSERT_ONLY result = VK_SUCCESS;
        
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
        assert(result == VK_SUCCESS);
    }

    void VulkanCore::initCommandBuffer()
    {
        VkResult U_ASSERT_ONLY result = VK_SUCCESS; 
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.pNext = nullptr;
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
        assert(result == VK_SUCCESS);
    }

    void VulkanCore::executeBeginCommandBuffer() 
    {
        VkResult U_ASSERT_ONLY result = VK_SUCCESS;

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.pNext = nullptr;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
        assert(result == VK_SUCCESS);
    }

    void VulkanCore::executeEndCommandBuffer() 
    {
        VkResult U_ASSERT_ONLY result = VK_SUCCESS;

        result = vkEndCommandBuffer(commandBuffer);
        assert(result == VK_SUCCESS);
    }

    void VulkanCore::initDeviceQueue() 
    {   /* DEPENDS on init_swapchain_extension() */

        vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
        if (graphicsQueueFamilyIndex == presentQueueFamilyIndex) 
        {
            presentQueue = graphicsQueue;
        } 
        else 
        {
            vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
        }
    }

    void VulkanCore::initSwapChain(VkImageUsageFlags usageFlags) 
    {   /* DEPENDS on commandBuffer and queue initialized */

        VkResult U_ASSERT_ONLY result = VK_SUCCESS;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;

        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[0], surface, &surfaceCapabilities);
        assert(result == VK_SUCCESS);

        uint32_t presentModeCount;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, nullptr);
        assert(result == VK_SUCCESS);
        VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
        assert(presentModes);
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, presentModes);
        assert(result == VK_SUCCESS);

        VkExtent2D swapChainExtent;
        // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
        if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) 
        {
            // If the surface size is undefined, the size is set to
            // the size of the images requested.
            swapChainExtent.width = window->getWidth();
            swapChainExtent.height = window->getHeight();
            if (swapChainExtent.width < surfaceCapabilities.minImageExtent.width) 
            {
                swapChainExtent.width = surfaceCapabilities.minImageExtent.width;
            } 
            else if (swapChainExtent.width > surfaceCapabilities.maxImageExtent.width) 
            {
                swapChainExtent.width = surfaceCapabilities.maxImageExtent.width;
            }

            if (swapChainExtent.height < surfaceCapabilities.minImageExtent.height) 
            {
                swapChainExtent.height = surfaceCapabilities.minImageExtent.height;
            } 
            else if (swapChainExtent.height > surfaceCapabilities.maxImageExtent.height) 
            {
                swapChainExtent.height = surfaceCapabilities.maxImageExtent.height;
            }
        } 
        else 
        {
            // If the surface size is defined, the swap chain size must match
            swapChainExtent = surfaceCapabilities.currentExtent;
        }

        // The FIFO present mode is guaranteed by the spec to be supported
        // Also note that current Android driver only supports FIFO
        VkPresentModeKHR swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        // Determine the number of VkImage's to use in the swap chain.
        // We need to acquire only 1 presentable image at at time.
        // Asking for minImageCount images ensures that we can acquire
        // 1 presentable image as long as we present it before attempting
        // to acquire another.
        uint32_t desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount;

        VkSurfaceTransformFlagBitsKHR preTransform;
        if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) 
        {
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        } 
        else 
        {
            preTransform = surfaceCapabilities.currentTransform;
        }

        // Find a supported composite alpha mode - one of these is guaranteed to be set
        VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = 
        {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };
        for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) 
        {
            if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) 
            {
                compositeAlpha = compositeAlphaFlags[i];
                break;
            }
        }

        VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.pNext = nullptr;
        swapChainCreateInfo.surface = surface;
        swapChainCreateInfo.minImageCount = desiredNumberOfSwapChainImages;
        swapChainCreateInfo.imageFormat = format;
        swapChainCreateInfo.imageExtent.width = swapChainExtent.width;
        swapChainCreateInfo.imageExtent.height = swapChainExtent.height;
        swapChainCreateInfo.preTransform = preTransform;
        swapChainCreateInfo.compositeAlpha = compositeAlpha;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.presentMode = swapChainPresentMode;
        swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
#ifndef __ANDROID__
        swapChainCreateInfo.clipped = true;
#else
        swapChainCreateInfo.clipped = false;
#endif
        swapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        swapChainCreateInfo.imageUsage = usageFlags;
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
        uint32_t queueFamilyIndices[2] = {(uint32_t)graphicsQueueFamilyIndex, (uint32_t)presentQueueFamilyIndex};
        if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) 
        {
            // If the graphics and present queues are from different queue families,
            // we either have to explicitly transfer ownership of images between the
            // queues, or we have to create the swapchain with imageSharingMode
            // as VK_SHARING_MODE_CONCURRENT
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }

        result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain);
        assert(result == VK_SUCCESS);

        result = vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, nullptr);
        assert(result == VK_SUCCESS);

        VkImage *swapChainImages = (VkImage *)malloc(swapChainImageCount * sizeof(VkImage));
        assert(swapChainImages);
        result = vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImages);
        assert(result == VK_SUCCESS);

        for (uint32_t i = 0; i < swapChainImageCount; i++) 
        {
            SwapChainBuffer swapChainBuffer;

            VkImageViewCreateInfo colorImageViewCreateInfo = {};
            colorImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            colorImageViewCreateInfo.pNext = nullptr;
            colorImageViewCreateInfo.format = format;
            colorImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            colorImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            colorImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            colorImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            colorImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            colorImageViewCreateInfo.subresourceRange.levelCount = 1;
            colorImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            colorImageViewCreateInfo.subresourceRange.layerCount = 1;
            colorImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorImageViewCreateInfo.flags = 0;

            swapChainBuffer.image = swapChainImages[i];

            colorImageViewCreateInfo.image = swapChainBuffer.image;

            result = vkCreateImageView(device, &colorImageViewCreateInfo, nullptr, &swapChainBuffer.view);
            swapChainBuffers.push_back(swapChainBuffer);
            assert(result == VK_SUCCESS);
        }
        free(swapChainImages);
        currentBuffer = 0;

        if (nullptr != presentModes) 
        {
            free(presentModes);
        }
    }
    
    void VulkanCore::initDepthBuffer() 
    {
        VkResult U_ASSERT_ONLY result = VK_SUCCESS;
        bool U_ASSERT_ONLY pass = true;
        VkImageCreateInfo imageCreateInfo = {};

        // allow custom depth formats 
#ifdef __ANDROID__
        // Depth format needs to be VK_FORMAT_D24_UNORM_S8_UINT on Android.
        depthBufferFormat = VK_FORMAT_D24_UNORM_S8_UINT;
#elif defined(VK_USE_PLATFORM_IOS_MVK)
        if (depthBufferFormat == VK_FORMAT_UNDEFINED) depthBufferFormat = VK_FORMAT_D32_SFLOAT;
#else
        if (depthBufferFormat == VK_FORMAT_UNDEFINED) depthBufferFormat = VK_FORMAT_D16_UNORM;
#endif

        const VkFormat depthFormat = depthBufferFormat;
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(gpus[0], depthFormat, &formatProperties);
        if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
        {
            imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
        } 
        else if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
        {
            imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        } else 
        {
            // Try other depth formats? 
            std::cout << "depth_format " << depthFormat << " Unsupported.\n";
            exit(-1);
        }

        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = depthFormat;
        imageCreateInfo.extent.width = window->getWidth();
        imageCreateInfo.extent.height = window->getHeight();
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = NUM_SAMPLES;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.queueFamilyIndexCount = 0;
        imageCreateInfo.pQueueFamilyIndices = nullptr;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageCreateInfo.flags = 0;

        VkMemoryAllocateInfo memoryAllocationInfo = {};
        memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocationInfo.pNext = nullptr;
        memoryAllocationInfo.allocationSize = 0;
        memoryAllocationInfo.memoryTypeIndex = 0;

        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.image = VK_NULL_HANDLE;
        imageViewCreateInfo.format = depthFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.flags = 0;

        if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT || depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
            depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) 
        {
            imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        VkMemoryRequirements memoryRequirements;

        // Create image 
        result = vkCreateImage(device, &imageCreateInfo, nullptr, &depthBuffer.image);
        assert(result == VK_SUCCESS);

        vkGetImageMemoryRequirements(device, depthBuffer.image, &memoryRequirements);

        memoryAllocationInfo.allocationSize = memoryRequirements.size;
        // Use the memory properties to determine the type of memory required 
        pass =
            memoryTypeFromProperties(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memoryAllocationInfo.memoryTypeIndex);
        assert(pass);

        // Allocate memory 
        result = vkAllocateMemory(device, &memoryAllocationInfo, nullptr, &depthBuffer.memory);
        assert(result == VK_SUCCESS);

        // Bind memory 
        result = vkBindImageMemory(device, depthBuffer.image, depthBuffer.memory, 0);
        assert(result == VK_SUCCESS);

        // Create image view 
        imageViewCreateInfo.image = depthBuffer.image;
        result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &depthBuffer.view);
        assert(result == VK_SUCCESS);
    }

    bool VulkanCore::memoryTypeFromProperties(uint32_t typeBits,
                                 VkFlags requirementsMask,
                                 uint32_t *typeIndex)
    {
        // Search memtypes to find first index with those properties
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) 
        {
            if ((typeBits & 1) == 1) 
            {
                // Type is available, does it match user properties?
                if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) 
                {
                    *typeIndex = i;
                    return true;
                }
            }
            typeBits >>= 1;
        }
        // No memory types matched, return failure
        return false;
    }
    
    // TODO: this is stuff for the camera. the camera should be refactored into a separate class
    void VulkanCore::initUniformBuffer()
    {
        VkResult U_ASSERT_ONLY result = VK_SUCCESS;
        bool U_ASSERT_ONLY pass = true;
        float fov = glm::radians(45.0f);
        if (window->getWidth() > window->getHeight()) 
        {
            fov *= static_cast<float>(window->getHeight()) / static_cast<float>(window->getWidth());
        }
        projectionMatrix = glm::perspective(fov, static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight()), 0.1f, 100.0f);
        viewMatrix = glm::lookAt(glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
                                 glm::vec3(0, 0, 0),     // and looks at the origin
                                 glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
                                 );
        modelMatrix = glm::mat4(1.0f);
        // Vulkan clip space has inverted Y and half Z.
        clipMatrix = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

        modelViewProjectionMatrix = clipMatrix * projectionMatrix * viewMatrix * modelMatrix;

        /* VULKAN_KEY_START */
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferCreateInfo.size = sizeof(modelViewProjectionMatrix);
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.flags = 0;
        result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &uniformData.buffer);
        assert(result == VK_SUCCESS);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, uniformData.buffer, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocationInfo = {};
        memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocationInfo.pNext = nullptr;
        memoryAllocationInfo.memoryTypeIndex = 0;

        memoryAllocationInfo.allocationSize = memoryRequirements.size;
        pass = memoryTypeFromProperties(memoryRequirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        &memoryAllocationInfo.memoryTypeIndex);
        assert(pass && "No mappable, coherent memory");

        result = vkAllocateMemory(device, &memoryAllocationInfo, nullptr, &(uniformData.memory));
        assert(result == VK_SUCCESS);

        uint8_t *pData;
        result = vkMapMemory(device, uniformData.memory, 0, memoryRequirements.size, 0, (void **)&pData);
        assert(result == VK_SUCCESS);

        memcpy(pData, &modelViewProjectionMatrix, sizeof(modelViewProjectionMatrix));

        vkUnmapMemory(device, uniformData.memory);

        result = vkBindBufferMemory(device, uniformData.buffer, uniformData.memory, 0);
        assert(result == VK_SUCCESS);

        uniformData.bufferInfo.buffer = uniformData.buffer;
        uniformData.bufferInfo.offset = 0;
        uniformData.bufferInfo.range = sizeof(modelViewProjectionMatrix);
    }
    void VulkanCore::initDescriptorAndPipelineLayouts(
        bool useTexture,
        VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags)
    {
        VkDescriptorSetLayoutBinding layoutBindings[2];
        layoutBindings[0].binding = 0;
        layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBindings[0].descriptorCount = 1;
        layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        layoutBindings[0].pImmutableSamplers = nullptr;

        if(useTexture)
        {
            layoutBindings[1].binding = 1;
            layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layoutBindings[1].descriptorCount = 1;
            layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            layoutBindings[1].pImmutableSamplers = nullptr;
        }

        // Next take layout bindings and use them to create a descriptor set layout
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.pNext = nullptr;
        descriptorSetLayoutCreateInfo.flags = descriptorSetLayoutCreateFlags;
        descriptorSetLayoutCreateInfo.bindingCount  =useTexture ? 2 : 1;
        descriptorSetLayoutCreateInfo.pBindings = layoutBindings;

        VkResult U_ASSERT_ONLY result = VK_SUCCESS;

        descriptorSetLayout.resize(NUM_DESCRIPTOR_SETS);
        result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayout.data());
        assert(result == VK_SUCCESS);

        // Now use the descriptor layout to create a pipelineCreateInfo layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.data();

        result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
        assert(result == VK_SUCCESS);
    }

    void VulkanCore::initRenderpass(bool includeDepth, bool clear, VkImageLayout finalLayout)
    {   // DEPENDS on init_swap_chain() and init_depth_buffer()

        VkResult U_ASSERT_ONLY result;
        // Need imageViewAttachments for render target and depth buffer
        VkAttachmentDescription imageViewAttachments[2];
        imageViewAttachments[0].format = format;
        imageViewAttachments[0].samples = NUM_SAMPLES;
        imageViewAttachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        imageViewAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        imageViewAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        imageViewAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        imageViewAttachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageViewAttachments[0].finalLayout = finalLayout;
        imageViewAttachments[0].flags = 0;

        if (includeDepth) 
        {
            imageViewAttachments[1].format = depthBufferFormat;
            imageViewAttachments[1].samples = NUM_SAMPLES;
            imageViewAttachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            imageViewAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            imageViewAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            imageViewAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            imageViewAttachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageViewAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            imageViewAttachments[1].flags = 0;
        }

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentReference = {};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.flags = 0;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;
        subpass.pResolveAttachments = nullptr;
        subpass.pDepthStencilAttachment = includeDepth ? &depthAttachmentReference : nullptr;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = nullptr;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.pNext = nullptr;
        renderPassCreateInfo.attachmentCount = includeDepth ? 2 : 1;
        renderPassCreateInfo.pAttachments = imageViewAttachments;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 0;
        renderPassCreateInfo.pDependencies = nullptr;

        result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
        assert(result == VK_SUCCESS);
    }
 

    void VulkanCore::initShaders(const char *vertexShaderText, const char *fragmentShaderText)
    { 
        VkResult U_ASSERT_ONLY result = VK_SUCCESS;
        bool U_ASSERT_ONLY returnValue = true;

        // If no shaders were submitted, just return
        if (!(vertexShaderText || fragmentShaderText)) 
        {
            std::cout << "shaders not complete" << std::endl;
            return;
        }

        initGlslang();

        VkShaderModuleCreateInfo moduleCreateInfo;

        if (vertexShaderText) 
        {
            std::vector<unsigned int> vertexShaderSpv;
            shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[0].pNext = nullptr;
            shaderStages[0].pSpecializationInfo = nullptr;
            shaderStages[0].flags = 0;
            shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStages[0].pName = "main";

            returnValue = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderText, vertexShaderSpv);
            assert(returnValue);

            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.pNext = nullptr;
            moduleCreateInfo.flags = 0;
            moduleCreateInfo.codeSize = vertexShaderSpv.size() * sizeof(unsigned int);
            moduleCreateInfo.pCode = vertexShaderSpv.data();
            result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderStages[0].module);
            assert(result == VK_SUCCESS);
        }

        if (fragmentShaderText) 
        {
            std::vector<unsigned int> fragmentShaderSpv;
            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].pNext = nullptr;
            shaderStages[1].pSpecializationInfo = nullptr;
            shaderStages[1].flags = 0;
            shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStages[1].pName = "main";

            returnValue = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderText, fragmentShaderSpv);
            assert(returnValue);

            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.pNext = nullptr;
            moduleCreateInfo.flags = 0;
            moduleCreateInfo.codeSize = fragmentShaderSpv.size() * sizeof(unsigned int);
            moduleCreateInfo.pCode = fragmentShaderSpv.data();
            result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderStages[1].module);
            assert(result == VK_SUCCESS);
        }

        finalizeGlslang();
    }

    void VulkanCore::initFrameBuffers(bool includeDepth)
    {
        // DEPENDS on init_depth_buffer(), init_renderpass() and
        // init_swapchain_extension() 

        VkResult U_ASSERT_ONLY result;
        VkImageView imageViewAttachments[2];
        imageViewAttachments[1] = depthBuffer.view;

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.pNext = nullptr;
        frameBufferCreateInfo.renderPass = renderPass;
        frameBufferCreateInfo.attachmentCount = includeDepth ? 2 : 1;
        frameBufferCreateInfo.pAttachments = imageViewAttachments;
        frameBufferCreateInfo.width = window->getWidth();
        frameBufferCreateInfo.height = window->getHeight();
        frameBufferCreateInfo.layers = 1;

        frameBuffers = (VkFramebuffer *)malloc(swapChainImageCount * sizeof(VkFramebuffer));

        for (uint32_t i = 0; i < swapChainImageCount; i++) 
        {
            imageViewAttachments[0] = swapChainBuffers[i].view;
            result = vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &frameBuffers[i]);
            assert(result == VK_SUCCESS);
        }
    }
    void VulkanCore::initVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture)
    {
        VkResult U_ASSERT_ONLY result;
        bool U_ASSERT_ONLY pass;

        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferCreateInfo.size = dataSize;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.flags = 0;
        result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &vertexBuffer.buffer);
        assert(result == VK_SUCCESS);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, vertexBuffer.buffer, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocationInfo = {};
        memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocationInfo.pNext = nullptr;
        memoryAllocationInfo.memoryTypeIndex = 0;

        memoryAllocationInfo.allocationSize = memoryRequirements.size;
        pass = memoryTypeFromProperties(memoryRequirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        &memoryAllocationInfo.memoryTypeIndex);
        assert(pass && "No mappable, coherent memory");

        result = vkAllocateMemory(device, &memoryAllocationInfo, nullptr, &(vertexBuffer.memory));
        assert(result == VK_SUCCESS);
        vertexBuffer.bufferInfo.range = memoryRequirements.size;
        vertexBuffer.bufferInfo.offset = 0;

        uint8_t *pData;
        result = vkMapMemory(device, vertexBuffer.memory, 0, memoryRequirements.size, 0, (void **)&pData);
        assert(result == VK_SUCCESS);

        memcpy(pData, vertexData, dataSize);

        vkUnmapMemory(device, vertexBuffer.memory);

        result = vkBindBufferMemory(device, vertexBuffer.buffer, vertexBuffer.memory, 0);
        assert(result == VK_SUCCESS);

        vertexInputBinding.binding = 0;
        vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBinding.stride = dataStride;

        vertexInputAttributes[0].binding = 0;
        vertexInputAttributes[0].location = 0;
        vertexInputAttributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexInputAttributes[0].offset = 0;
        vertexInputAttributes[1].binding = 0;
        vertexInputAttributes[1].location = 1;
        vertexInputAttributes[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexInputAttributes[1].offset = 16;
    }
    void VulkanCore::initDescriptorPool(bool useTexture)
    {
        // DEPENDS on init_uniform_buffer() and
        // init_descriptor_and_pipeline_layouts() 

        VkResult U_ASSERT_ONLY result;
        VkDescriptorPoolSize typeCount[2];
        typeCount[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        typeCount[0].descriptorCount = 1;
        if (useTexture) 
        {
            typeCount[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            typeCount[1].descriptorCount = 1;
        }

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext = nullptr;
        descriptorPoolCreateInfo.maxSets = 1;
        descriptorPoolCreateInfo.poolSizeCount = useTexture ? 2 : 1;
        descriptorPoolCreateInfo.pPoolSizes = typeCount;

        result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
        assert(result == VK_SUCCESS);
    }
    void VulkanCore::initDescriptorSet(bool useTexture)
    {
        // DEPENDS on init_descriptor_pool()

        VkResult U_ASSERT_ONLY result;

        VkDescriptorSetAllocateInfo descriptorsetAllocationInfo[1];
        descriptorsetAllocationInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorsetAllocationInfo[0].pNext = nullptr;
        descriptorsetAllocationInfo[0].descriptorPool = descriptorPool;
        descriptorsetAllocationInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
        descriptorsetAllocationInfo[0].pSetLayouts = descriptorSetLayout.data();

        descriptorSets.resize(NUM_DESCRIPTOR_SETS);
        result = vkAllocateDescriptorSets(device, descriptorsetAllocationInfo, descriptorSets.data());
        assert(result == VK_SUCCESS);

        VkWriteDescriptorSet writes[2];

        writes[0] = {};
        writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[0].pNext = nullptr;
        writes[0].dstSet = descriptorSets[0];
        writes[0].descriptorCount = 1;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].pBufferInfo = &uniformData.bufferInfo;
        writes[0].dstArrayElement = 0;
        writes[0].dstBinding = 0;

        if (useTexture) 
        {
            writes[1] = {};
            writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[1].dstSet = descriptorSets[0];
            writes[1].dstBinding = 1;
            writes[1].descriptorCount = 1;
            writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[1].pImageInfo = &textureData.imageInfo;
            writes[1].dstArrayElement = 0;
        }

        vkUpdateDescriptorSets(device, useTexture ? 2 : 1, writes, 0, nullptr);
    }
    void VulkanCore::initPipelineCache()
    {
        VkResult U_ASSERT_ONLY result;

        VkPipelineCacheCreateInfo pipelineCacheCreateInfo;
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        pipelineCacheCreateInfo.pNext = nullptr;
        pipelineCacheCreateInfo.initialDataSize = 0;
        pipelineCacheCreateInfo.pInitialData = nullptr;
        pipelineCacheCreateInfo.flags = 0;
        result = vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
        assert(result == VK_SUCCESS);
    }
    void VulkanCore::initPipeline(VkBool32 includeDepth, VkBool32 includeVertexInput)
    {
        VkResult U_ASSERT_ONLY result;

        VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pNext = nullptr;
        dynamicState.pDynamicStates = dynamicStateEnables;
        dynamicState.dynamicStateCount = 0;

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
        memset(&vertexInputStateCreateInfo, 0, sizeof(vertexInputStateCreateInfo));
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        if (includeVertexInput) 
        {
            vertexInputStateCreateInfo.pNext = nullptr;
            vertexInputStateCreateInfo.flags = 0;
            vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
            vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBinding;
            vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
            vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributes;
        }
        VkPipelineInputAssemblyStateCreateInfo inpitAssemblyStateCreateInfo;
        inpitAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inpitAssemblyStateCreateInfo.pNext = nullptr;
        inpitAssemblyStateCreateInfo.flags = 0;
        inpitAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
        inpitAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.pNext = nullptr;
        rasterizationStateCreateInfo.flags = 0;
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0;
        rasterizationStateCreateInfo.depthBiasClamp = 0;
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0;
        rasterizationStateCreateInfo.lineWidth = 1.0f;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.flags = 0;
        colorBlendStateCreateInfo.pNext = nullptr;
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState[1];
        colorBlendAttachmentState[0].colorWriteMask = 0xf;
        colorBlendAttachmentState[0].blendEnable = VK_FALSE;
        colorBlendAttachmentState[0].alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState[0].colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = colorBlendAttachmentState;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
        colorBlendStateCreateInfo.blendConstants[0] = 1.0f;
        colorBlendStateCreateInfo.blendConstants[1] = 1.0f;
        colorBlendStateCreateInfo.blendConstants[2] = 1.0f;
        colorBlendStateCreateInfo.blendConstants[3] = 1.0f;

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.pNext = nullptr;
        viewportStateCreateInfo.flags = 0;
#ifndef __ANDROID__
        viewportStateCreateInfo.viewportCount = NUM_VIEWPORTS;
        dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
        viewportStateCreateInfo.scissorCount = NUM_SCISSORS;
        dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
        viewportStateCreateInfo.pScissors = nullptr;
        viewportStateCreateInfo.pViewports = nullptr;
#else
        // Temporary disabling dynamic viewport on Android because some of drivers doesn't
        // support the feature.
        VkViewport viewports;
        viewports.minDepth = 0.0f;
        viewports.maxDepth = 1.0f;
        viewports.x = 0;
        viewports.y = 0;
        viewports.width = window->getWidth();
        viewports.height = window->getHeight();
        VkRect2D scissor;
        scissor.extent.width = window->getWidth();
        scissor.extent.height = window->getHeight();
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        viewportStateCreateInfo.viewportCount = NUM_VIEWPORTS;
        viewportStateCreateInfo.scissorCount = NUM_SCISSORS;
        viewportStateCreateInfo.pScissors = &scissor;
        viewportStateCreateInfo.pViewports = &viewports;
    #endif
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.pNext = nullptr;
        depthStencilStateCreateInfo.flags = 0;
        depthStencilStateCreateInfo.depthTestEnable = includeDepth;
        depthStencilStateCreateInfo.depthWriteEnable = includeDepth;
        depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
        depthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
        depthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStencilStateCreateInfo.back.compareMask = 0;
        depthStencilStateCreateInfo.back.reference = 0;
        depthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
        depthStencilStateCreateInfo.back.writeMask = 0;
        depthStencilStateCreateInfo.minDepthBounds = 0;
        depthStencilStateCreateInfo.maxDepthBounds = 0;
        depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.front = depthStencilStateCreateInfo.back;

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.pNext = nullptr;
        multisampleStateCreateInfo.flags = 0;
        multisampleStateCreateInfo.pSampleMask = nullptr;
        multisampleStateCreateInfo.rasterizationSamples = NUM_SAMPLES;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
        multisampleStateCreateInfo.minSampleShading = 0.0;

        VkGraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext = nullptr;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = 0;
        pipelineCreateInfo.flags = 0;
        pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inpitAssemblyStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pTessellationState = nullptr;
        pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        pipelineCreateInfo.pDynamicState = &dynamicState;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
        pipelineCreateInfo.pStages = shaderStages;
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.subpass = 0;

        result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline);
        assert(result == VK_SUCCESS);
    }
}
