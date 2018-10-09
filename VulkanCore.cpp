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
        if(depthBuffer.mem)
            vkFreeMemory(device, depthBuffer.mem, nullptr);
        if(swapChain)
        {
            for(uint32_t i = 0; i < swapChainImageCount; i++)
            {
                vkDestroyImageView(device, buffers[i].view, nullptr);
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
        auto res = initGlobalLayerProperties();
        if(res != VK_SUCCESS)
        {
            std::cout << "Failed to init GlobalLayerProperties. " 
               << "Res: " << res << std::endl;
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
        VkResult res = VK_SUCCESS;
        uint32_t instanceLayerCount;
        VkLayerProperties *vkLayerProperties = nullptr;
        do
        {
            res = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
            if(res)
            {
                return res;
            }

            if(instanceLayerCount == 0)
            {
                return VK_SUCCESS;
            }

            vkLayerProperties = (VkLayerProperties*)realloc(
                    vkLayerProperties, 
                    instanceLayerCount * sizeof(VkLayerProperties));
            
            res = vkEnumerateInstanceLayerProperties(
                    &instanceLayerCount,
                    vkLayerProperties);
        }
        while(res == VK_INCOMPLETE);
        
        for(uint32_t i = 0; i < instanceLayerCount; i++)
        {
            LayerProperties layerProperties;
            layerProperties.properties = vkLayerProperties[i];
            res = initGlobalExtensionProperties(layerProperties);
            if(res)
                return res;
            instanceLayerProperties.push_back(layerProperties);
        }
        free(vkLayerProperties);

        return res;
    }

    VkResult VulkanCore::initGlobalExtensionProperties(LayerProperties &layerProperties)
    {
        VkExtensionProperties *instanceExtensions;
        uint32_t instanceExtensionCount;
        VkResult res = VK_SUCCESS;

        auto layerName = layerProperties.properties.layerName;
        
        do
        {
            res = vkEnumerateInstanceExtensionProperties(
                    layerName,
                    &instanceExtensionCount,
                    nullptr);
            
            if(res)
            {
                return res;
            }

            if(instanceExtensionCount == 0)
            {
                return VK_SUCCESS;
            }

            layerProperties.instanceExtensions.resize(instanceExtensionCount);
            instanceExtensions = layerProperties.instanceExtensions.data();
            res = vkEnumerateInstanceExtensionProperties(
                    layerName,
                    &instanceExtensionCount,
                    instanceExtensions);
        }
        while(res == VK_INCOMPLETE);

        return res;
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
        VkResult res = VK_SUCCESS;
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

        res = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        assert(res == VK_SUCCESS);

        return res;
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
        VkResult res = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
        assert(gpuCount);
        gpus.resize(gpuCount);

        res = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
        assert(!res && gpuCount >= requiredGpuCount);

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

        return res;
    }

    VkResult VulkanCore::initDeviceExtensionProperties(LayerProperties &layerProperties)
    {
        VkResult res = VK_SUCCESS;
        
        auto layerName = layerProperties.properties.layerName;
        
        do
        {
            VkExtensionProperties *deviceExtensions;
            uint32_t deviceExtensionCount;

            res = vkEnumerateDeviceExtensionProperties(
                    gpus[0], 
                    layerName, 
                    &deviceExtensionCount,
                    nullptr);

            if(res)
            {
                return res;
            }

            if(deviceExtensionCount == 0)
            {
                return VK_SUCCESS;
            }

            layerProperties.deviceExtensions.resize(deviceExtensionCount);
            deviceExtensions = layerProperties.deviceExtensions.data();
            res = vkEnumerateDeviceExtensionProperties(
                    gpus[0], 
                    layerName,
                    &deviceExtensionCount,
                    deviceExtensions);                    
        }
        while(res == VK_INCOMPLETE);

        return res;
    }

    void VulkanCore::initSwapchainExtension()
    {
        VkResult U_ASSERT_ONLY res = VK_SUCCESS;

#ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.hinstance = window->getConnection();
        surfaceCreateInfo.hwnd = window->getWindow();
        res = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(__ANDROID__)
        // this is not yet supported
        /*
        GET_INSTANCE_PROC_ADDR(instance, CreateAndroidSurfaceKHR);

        VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.window = AndroidGetApplicationWindow();
        res = fpCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
        */
#elif defined(VK_USE_PLATFOR_IOS_MVK)
        VkIOSSurfaceInfoMVK surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_IOS_CREATE_INFO_MVK;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.pView = window->getWindow();
        res = vkCreateIOSSurfaceMVK(instance, &surfaceCreateInfo, nullptr, &surface);
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
        res = vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#else
        VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.connection = window->getConnection();
        surfaceCreateInfo.window = window->getWindow();
        res = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#endif
        assert(res == VK_SUCCESS);
        
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
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, nullptr);
        assert(res == VK_SUCCESS);
        VkSurfaceFormatKHR *surfaceFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, surfaceFormats);
        assert(res == VK_SUCCESS);

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
        VkResult res = VK_SUCCESS;
        
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

        res = vkCreateDevice(gpus[0], &deviceCreateInfo, nullptr, &device);
        assert(res == VK_SUCCESS);

        return res;
    }

    void VulkanCore::initCommandPool()
    {
        VkResult U_ASSERT_ONLY res = VK_SUCCESS;
        
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        res = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
        assert(res == VK_SUCCESS);
    }

    void VulkanCore::initCommandBuffer()
    {
        VkResult U_ASSERT_ONLY res = VK_SUCCESS; 
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.pNext = nullptr;
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        res = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
        assert(res == VK_SUCCESS);
    }

    void VulkanCore::executeBeginCommandBuffer() 
    {
        VkResult U_ASSERT_ONLY res = VK_SUCCESS;

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.pNext = nullptr;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        res = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
        assert(res == VK_SUCCESS);
    }

    void VulkanCore::executeEndCommandBuffer() 
    {
        VkResult U_ASSERT_ONLY res = VK_SUCCESS;

        res = vkEndCommandBuffer(commandBuffer);
        assert(res == VK_SUCCESS);
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

        VkResult U_ASSERT_ONLY res = VK_SUCCESS;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;

        res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[0], surface, &surfaceCapabilities);
        assert(res == VK_SUCCESS);

        uint32_t presentModeCount;
        res = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, nullptr);
        assert(res == VK_SUCCESS);
        VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
        assert(presentModes);
        res = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[0], surface, &presentModeCount, presentModes);
        assert(res == VK_SUCCESS);

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

        res = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain);
        assert(res == VK_SUCCESS);

        res = vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, nullptr);
        assert(res == VK_SUCCESS);

        VkImage *swapChainImages = (VkImage *)malloc(swapChainImageCount * sizeof(VkImage));
        assert(swapChainImages);
        res = vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImages);
        assert(res == VK_SUCCESS);

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

            res = vkCreateImageView(device, &colorImageViewCreateInfo, nullptr, &swapChainBuffer.view);
            buffers.push_back(swapChainBuffer);
            assert(res == VK_SUCCESS);
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
        VkResult U_ASSERT_ONLY res = VK_SUCCESS;
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
        res = vkCreateImage(device, &imageCreateInfo, nullptr, &depthBuffer.image);
        assert(res == VK_SUCCESS);

        vkGetImageMemoryRequirements(device, depthBuffer.image, &memoryRequirements);

        memoryAllocationInfo.allocationSize = memoryRequirements.size;
        // Use the memory properties to determine the type of memory required 
        pass =
            memoryTypeFromProperties(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memoryAllocationInfo.memoryTypeIndex);
        assert(pass);

        // Allocate memory 
        res = vkAllocateMemory(device, &memoryAllocationInfo, nullptr, &depthBuffer.mem);
        assert(res == VK_SUCCESS);

        // Bind memory 
        res = vkBindImageMemory(device, depthBuffer.image, depthBuffer.mem, 0);
        assert(res == VK_SUCCESS);

        // Create image view 
        imageViewCreateInfo.image = depthBuffer.image;
        res = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &depthBuffer.view);
        assert(res == VK_SUCCESS);
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
        VkResult U_ASSERT_ONLY res = VK_SUCCESS;
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
        res = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &uniformData.buffer);
        assert(res == VK_SUCCESS);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, uniformData.buffer, &memoryRequirements);

        VkMemoryAllocateInfo memoryAllocationInfo = {};
        memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocationInfo.pNext = NULL;
        memoryAllocationInfo.memoryTypeIndex = 0;

        memoryAllocationInfo.allocationSize = memoryRequirements.size;
        pass = memoryTypeFromProperties(memoryRequirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        &memoryAllocationInfo.memoryTypeIndex);
        assert(pass && "No mappable, coherent memory");

        res = vkAllocateMemory(device, &memoryAllocationInfo, nullptr, &(uniformData.memory));
        assert(res == VK_SUCCESS);

        uint8_t *pData;
        res = vkMapMemory(device, uniformData.memory, 0, memoryRequirements.size, 0, (void **)&pData);
        assert(res == VK_SUCCESS);

        memcpy(pData, &modelViewProjectionMatrix, sizeof(modelViewProjectionMatrix));

        vkUnmapMemory(device, uniformData.memory);

        res = vkBindBufferMemory(device, uniformData.buffer, uniformData.memory, 0);
        assert(res == VK_SUCCESS);

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

        VkResult U_ASSERT_ONLY res = VK_SUCCESS;

        descriptorSetLayout.resize(NUM_DESCRIPTOR_SETS);
        res = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayout.data());
        assert(res == VK_SUCCESS);

        // Now use the descriptor layout to create a pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.data();

        res = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
        assert(res == VK_SUCCESS);
    }

    void VulkanCore::initRenderpass(bool includeDepth, bool clear, VkImageLayout finalLayout)
    {   // DEPENDS on init_swap_chain() and init_depth_buffer()

        VkResult U_ASSERT_ONLY res;
        // Need attachments for render target and depth buffer
        VkAttachmentDescription attachments[2];
        attachments[0].format = format;
        attachments[0].samples = NUM_SAMPLES;
        attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout = finalLayout;
        attachments[0].flags = 0;

        if (includeDepth) 
        {
            attachments[1].format = depthBufferFormat;
            attachments[1].samples = NUM_SAMPLES;
            attachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments[1].flags = 0;
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
        renderPassCreateInfo.pAttachments = attachments;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 0;
        renderPassCreateInfo.pDependencies = NULL;

        res = vkCreateRenderPass(device, &renderPassCreateInfo, NULL, &renderPass);
        assert(res == VK_SUCCESS);
    }
 

    void VulkanCore::initShaders(const char *vertexShaderText, const char *fragmentShaderText)
    { 
        VkResult U_ASSERT_ONLY res = VK_SUCCESS;
        bool U_ASSERT_ONLY retVal = true;

        // If no shaders were submitted, just return
        if (!(vertexShaderText || fragmentShaderText)) 
        {
            std::cout << "shaders not complete" << std::endl;
            return;
        }

        initGlslang();
    }

    void VulkanCore::initFrameBuffers(bool includeDepth)
    {}
    void VulkanCore::initVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture)
    {}
    void VulkanCore::initDescriptorPool(bool useTexture)
    {}
    void VulkanCore::initDescriptorSet(bool useTexture)
    {}
    void VulkanCore::initPipelineCache()
    {}
    void VulkanCore::initPipeline(bool includeDepth)
    {}
}
