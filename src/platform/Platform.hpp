#pragma once

#include <vulkan/vulkan.hpp>

#include "framework/Common.hpp"
#include "framework/TobiStatus.hpp"
#include "../framework/VkCommon.hpp"
#include "SwapChainDimensions.hpp"

namespace Tobi
{

class SemaphoreManager;

class Platform
{
  public:
    Platform(const Platform &) = delete;
    Platform(Platform &&) = delete;
    Platform &operator=(const Platform &) & = delete;
    Platform &operator=(Platform &&) & = delete;
    virtual ~Platform();

    void waitIdle();

    /// @brief Presents an image to the swapchain.
    /// @param index The swapchain index previously obtained from @ref
    /// acquireNextImage.
    /// @returns Error code.
    virtual Result presentImage(uint32_t index, const VkSemaphore &releaseSemaphore);

    Result acquireNextImage(uint32_t &swapChainIndex, VkSemaphore &acquireSemaphore);

    void addClearedSemaphore(VkSemaphore clearedSemaphore);

    /// @brief Returns the logical device.
    /// @returns The logical device, or nullptr if not set.
    inline const auto &getDevice() const
    {
        return logicalDevice;
    }

    inline const auto getSwapChainImageCount() const { return static_cast<uint32_t>(swapChainImages.size()); }

    inline const auto getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }

    inline const auto getGraphicsQueue() const { return graphicsQueue; }

    inline const auto &getSwapChainDimensions() const { return swapChainDimensions; }

    inline const auto &getSwapChainImages() const { return swapChainImages; }

    virtual const TobiStatus &getWindowStatus() const = 0;

    /// @brief Returns the currently set debug callback.
    /// @returns The callback, or nullptr if not set.
    inline PFN_vkDebugReportCallbackEXT getExternalDebugCallback() const { return externalDebugCallback; }

    /// @brief Returns the currently set debug callback.
    /// @returns The callback, or nullptr if not set.
    inline void *getExternalDebugCallbackUserData() const { return externalDebugCallbackUserData; }

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
                if ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & hostRequirements) == hostRequirements)
                {
                    return i;
                }
            }
        }

        LOGE("Failed to obtain suitable memory type.\n");
        abort();
    }

    uint32_t findMemoryTypeFromRequirementsWithFallback(uint32_t deviceRequirements,
                                                        uint32_t hostRequirements)
    {
        for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++)
        {
            if (deviceRequirements & (1u << i))
            {
                if ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & hostRequirements) == hostRequirements)
                {
                    return i;
                }
            }
        }

        // If we cannot find the particular memory type we're looking for, just pick the first one available.
        if (hostRequirements != 0)
            return findMemoryTypeFromRequirements(deviceRequirements, 0);
        else
        {
            LOGE("Failed to obtain suitable memory type.\n");
            abort();
        }
    }

    void createImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory);

    void transitionImageLayout(
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout);

    VkImageView createImageView(
        VkImage image,
        VkFormat format,
        VkImageAspectFlags aspectFlags);

    VkBool32 getSupportedDepthFormat(VkFormat *depthFormat)
    {
        // Since all depth formats may be optional, we need to find a suitable depth format to use
        // Start with the highest precision packed format
        std::vector<VkFormat> depthFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM};

        for (auto &format : depthFormats)
        {
            VkFormatProperties formatProps;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
            // Format must support depth stencil attachment for optimal tiling
            if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                *depthFormat = format;
                return true;
            }
        }

        return false;
    }

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
    int32_t graphicsQueueFamilyIndex;
    int32_t presentQueueFamilyIndex;
    int32_t computeQueueFamilyIndex;
    int32_t transferQueueFamilyIndex;
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

    std::unique_ptr<SemaphoreManager> semaphoreManager;

    /// Indicates if application can use the required extensions or will try without
    bool useInstanceExtensions;
    bool useDeviceExtensions;
    bool haveDebugReport;

    bool vsync;

    VkCommandPool commandPool;

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

    static std::shared_ptr<Platform> create();

    /// @brief Explicit tears down the swapchain.
    void destroySwapChain();

    virtual Result initialize() = 0;

    virtual Result initWindow() = 0;

    Result initInstance(
        const std::vector<const char *> &requiredInstanceExtensions);

    Result initDebugReport();

    Result initPhysicalDevice(
        const std::vector<const char *> &requiredDeviceExtensions);

    const VkPhysicalDevice pickPhysicalDevice() const;

    virtual Result initSurface() = 0;

    uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags);
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
