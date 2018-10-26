#pragma once

#include <vulkan/vulkan.hpp>

#include "../framework/Common.hpp"
#include "../framework/Context.hpp"

namespace Tobi
{

class Platform
{
  public:
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

    /// @brief Returns the currently set debug callback.
    /// @returns The callback, or nullptr if not set.
    inline PFN_vkDebugReportCallbackEXT getExternalDebugCallback() const { return externalDebugCallback; }

    /// @brief Returns the currently set debug callback.
    /// @returns The callback, or nullptr if not set.
    inline void *getExternalDebugCallbackUserData() const { return externalDebugCallbackUserData; }

  protected:
    Platform();

    virtual void terminate();

    /// The vulkan instance
    VkInstance instance;

    /// vulkan surface
    VkSurfaceKHR surface;

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

  private:
    friend class Context;

    /// The selected physical device
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

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

    /// logical device
    VkDevice logicalDevice;

    /// swapchain
    VkSwapchainKHR swapChain;
    SwapChainDimensions swapChainDimensions;
    std::vector<VkImage> swapChainImages;

    /// Indicates if application can use the required extensions or will try without
    bool useInstanceExtensions;
    bool useDeviceExtensions;
    bool haveDebugReport;

    /// List of external layers to load.
    std::vector<std::string> externalLayers;
    /// List of all active instance extensions
    std::vector<const char *> activeInstanceExtensions;
    /// List of all active instance layers
    std::vector<const char *> activeInstanceLayers;
    /// List of all active device layers
    std::vector<const char *> activeDeviceLayers;

    /// Debug Report calback function
    VkDebugReportCallbackEXT debugReportCallback;
    /// External debug callback.
    PFN_vkDebugReportCallbackEXT externalDebugCallback;
    /// User-data for external debug callback.
    void *externalDebugCallbackUserData;

    static std::unique_ptr<Platform> create();

    /// @brief Explicit tears down the swapchain.
    void destroySwapChain();

    virtual Result initialize() = 0;

    virtual Result initWindow() = 0;

    Result initInstance(
        const std::vector<const char *> &requiredInstanceExtensions);

    Result initDebugReport();

    Result initPhysicalDevice(
        const std::vector<const char *> &requiredDeviceExtensions);

    virtual Result initSurface() = 0;

    Result initQueueFamilyIndices();

    Result initDevice(
        const std::vector<const char *> &requiredDeviceExtensions);

    Result initSwapChain(const SwapChainDimensions &swapChainDimensions);

    Result initInstanceExtensions(
        const std::vector<const char *> &requiredInstanceExtensions);
    Result initDeviceExtensions(
        const std::vector<const char *> &requiredDeviceExtensions);

    bool validateExtensions(const std::vector<const char *> &requiredExtensions,
                            const std::vector<VkExtensionProperties> &availableExtensions);

    Result loadInstanceSymbols();
    Result loadDeviceSymbols();
};

} // namespace Tobi