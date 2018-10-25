#pragma once

#include <vulkan/vulkan.hpp>

#include "../framework/Common.hpp"
#include "../framework/Context.hpp"
#include "../framework/SemaphoreManager.hpp"

namespace Tobi
{

class Platform
{
  public:
    Platform();
    virtual ~Platform();

    /// @brief Describes the size and format of the swapchain.
    struct SwapChainDimensions
    {
        /// Width of the swapchain.
        uint32_t width;
        /// Height of the swapchain.
        uint32_t height;
        /// Pixel format of the swapchain.
        VkFormat format;
    };

    /// @brief Describes the status of the application lifecycle.
    enum Status
    {
        /// The application is running.
        STATUS_RUNNING,
        /// The application should exit as the user has requested it.
        STATUS_TEARDOWN
    };

    /// @brief Initializes the Vulkan device.
    /// @param swapchain The requested swapchain dimensions and size. Can be
    /// overridden by WSI.
    /// @param[out] instanceExtensions The required Vulkan instance extensions the
    /// platform requires.
    /// @param[out] deviceExtensions The required Vulkan device extensions the
    /// platform requires.
    /// @returns Error code
    Result initVulkan(
        const SwapChainDimensions &swapChainDimensions,
        const std::vector<const char *> &requiredInstanceExtensions,
        const std::vector<const char *> &requiredDeviceExtensions);

    /// @brief Gets the current Vulkan instance.
    /// @returns Vulkan instance.
    inline VkInstance getInstance() const
    {
        return instance;
    }

    std::shared_ptr<Context> &getContext()
    {
        return context;
    }
    /// @brief Returns the currently set debug callback.
    /// @returns The callback, or nullptr if not set.
    inline PFN_vkDebugReportCallbackEXT getExternalDebugCallback() const
    {
        return externalDebugCallback;
    }

    /// @brief Returns the currently set debug callback.
    /// @returns The callback, or nullptr if not set.
    inline void *getExternalDebugCallbackUserData() const
    {
        return pExternalDebugCallbackUserData;
    }

    inline const uint32_t getGraphicsQueueIndex() const
    {
        return graphicsQueueIndex;
    }

    inline const VkQueue &getGraphicsQueue() const
    {
        return graphicsQueue;
    }

    inline const VkDevice &getDevice() const
    {
        return logicalDevice;
    }

    inline const VkPhysicalDevice &getPhysicalDevice() const
    {
        return physicalDevice;
    }

    inline const uint32_t getNumSwapchainImages() const
    {
        return static_cast<uint32_t>(swapChainImages.size());
    }

    const SwapChainDimensions &getSwapChainDimensions() const { return swapChainDimensions; }
    const std::vector<VkImage> &getSwapChainImages() const { return swapChainImages; }

    Result acquireNextImage();

    /// @brief Terminates the platform. Normally this would be handled by the
    /// destructor, but certain platforms
    /// need to be able to terminate before exit() and initialize multiple times.
    virtual void terminate();

    /// @brief Gets the preferred swapchain size. Not relevant for all platforms.
    /// @returns Error code.
    virtual SwapChainDimensions getPreferredSwapChain() = 0;

    // To create a buffer, both the device and application have requirements from
    // the buffer object.
    // Vulkan exposes the different types of buffers the device can allocate, and we
    // have to find a suitable one.
    // deviceRequirements is a bitmask expressing which memory types can be used for
    // a buffer object.
    // The different memory types' properties must match with what the application
    // wants.
    uint32_t findMemoryTypeFromRequirements(uint32_t deviceRequirements, uint32_t hostRequirements)
    {
        for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++)
        {
            if (deviceRequirements & (1u << i))
            {
                if ((gpuMemoryProperties.memoryTypes[i].propertyFlags & hostRequirements) == hostRequirements)
                {
                    return i;
                }
            }
        }

        LOGE("Failed to obtain suitable memory type.\n");
        abort();
    }

  protected:
    /// The Vulkan instance.
    VkInstance instance;

    /// The selected physical device
    VkPhysicalDevice physicalDevice;

    /// The logical device
    VkDevice logicalDevice;

    VkPhysicalDeviceProperties gpuProperties;
    VkPhysicalDeviceMemoryProperties gpuMemoryProperties;

    // present graphics, transfer and compute queues. If the gpu supports it, they will be separate queues
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    VkQueueFlags supportedQueues;
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    uint32_t computeQueueIndex;
    uint32_t transferQueueIndex;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue computeQueue;
    VkQueue transferQueue;

    /// The vulkan context
    std::shared_ptr<Context> context;

    std::unique_ptr<SemaphoreManager> semaphoreManager;

    /// List of external layers to load.
    std::vector<std::string> externalLayers;

    /// Indicates if application can use the required extensions or will try without
    bool useInstanceExtensions;
    bool useDeviceExtensions;
    bool haveDebugReport;

    /// List of all active instance extensions
    std::vector<const char *> activeInstanceExtensions;

    /// List of all active instance layers
    std::vector<const char *> activeInstanceLayers;

    /// List of all active instance layers
    std::vector<const char *> activeDeviceLayers;

    /// Debug Report calback function
    VkDebugReportCallbackEXT debugReportCallback;

    /// External debug callback.
    PFN_vkDebugReportCallbackEXT externalDebugCallback;

    /// User-data for external debug callback.
    void *pExternalDebugCallbackUserData;

    /// @brief Helper function to add external layers to a list of active ones.
    /// @param activeInstanceLayers List of active layers to be used.
    /// @param supportedLayers List of supported layers.
    inline void addExternalLayers(std::vector<const char *> &activeInstanceLayers,
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

    /// @brief Explicit tears down the swapchain.
    void destroySwapChain();

  private:
    /// vulkan surface
    VkSurfaceKHR surface;

    /// swapchain
    VkSwapchainKHR swapChain;
    SwapChainDimensions swapChainDimensions;
    std::vector<VkImage> swapChainImages;
    uint32_t currentSwapChainIndex;

    Result initInstanceExtensions(
        const std::vector<const char *> &requiredInstanceExtensions);

    Result initDeviceExtensions(
        const std::vector<const char *> &requiredDeviceExtensions);

    Result initInstance();
    Result initDevice(
        const std::vector<const char *> &requiredDeviceExtensions);
    Result initPhysicalDevice();

    Result initSwapChain(const SwapChainDimensions &dim);

    virtual Result initWindow() = 0;
    virtual VkSurfaceKHR createSurface() = 0;

    Result initQueueFamilyIndices();

    void initDebugReport();

    bool validateExtensions(const std::vector<const char *> &requiredExtensions,
                            const std::vector<VkExtensionProperties> &availableExtensions);

    Result loadInstanceSymbols();
    Result loadDeviceSymbols();
};

} // namespace Tobi