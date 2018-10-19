#pragma once
#include <string>
#include <vector>
#include <memory>

#include "VulkanSwapChain.hpp"

#define LOGE(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
#define LOGI(...) fprintf(stderr, "INFO: " __VA_ARGS__)

namespace Tobi
{
enum Result
{
    RESULT_SUCCESS = 0,
    RESULT_ERROR_GENERIC = -1,
    RESULT_ERROR_OUTDATED_SWAPCHAIN = -2,
    RESULT_ERROR_IO = -3,
    RESULT_ERROR_OUT_OF_MEMORY = -4
};

class Platform
{
  public:
    Platform(const Platform &) = delete;
    Platform(Platform &&) = delete;
    Platform &operator=(const Platform &) & = delete;
    Platform &operator=(Platform &&) & = delete;
    virtual ~Platform();

    struct SwapChainDimensions
    {
        uint32_t width;
        uint32_t height;
        VkFormat format;
    };

    enum Status
    {
        STATUS_RUNNING,
        STATUS_TEARDOWN
    };

    virtual Result initialize() = 0;

    inline void addExternalLayerName(const char *layerName)
    {
        externalLayerNames.push_back(layerName);
    }

    inline void setExternalDebugCallback(PFN_vkDebugReportCallbackEXT callback, void *userData)
    {
        externalDebugCallback = callback;
        externalDebugCallbackUserData = userData;
    }
    inline PFN_vkDebugReportCallbackEXT getExternalDebugCallback() const
    {
        return externalDebugCallback;
    }
    inline void *getExternalDebugCallbackUserData() const
    {
        return externalDebugCallbackUserData;
    }

    virtual Result createWindow(const SwapChainDimensions &swapChainDimensions) = 0;

    void terminate();

  protected:
    Platform() = default;
    VkInstance instance;
    VkPhysicalDevice gpu;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    VkQueue computeQueue;
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    uint32_t transferQueueIndex;
    uint32_t computeQueueIndex;
    VkPhysicalDeviceProperties gpuProperties;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    std::vector<VkQueueFamilyProperties> queueProperties;
    std::vector<std::string> externalLayerNames;
    PFN_vkDebugReportCallbackEXT externalDebugCallback;
    void *externalDebugCallbackUserData;

    inline void addExternalLayers(
        std::vector<const char *> &activeLayers,
        const std::vector<VkLayerProperties> &supportedLayers)
    {
        for (auto &layerName : externalLayerNames)
        {
            for (auto &supportedLayer : supportedLayers)
            {
                if (layerName == supportedLayer.layerName)
                {
                    activeLayers.push_back(supportedLayer.layerName);
                    LOGI("Found external layerName: %s\n", supportedLayer.layerName);
                    break;
                }
            }
        }
    }

    Result initVulkan(
        const SwapChainDimensions &swapChainDimensions, // change to screendimensions?
        const std::vector<const char *> &instanceExtensions,
        const std::vector<const char *> &deviceExtensions);

  private:
    std::shared_ptr<VulkanSwapChain> swapChain;

    VkSurfaceKHR surface;

    VkDebugReportCallbackEXT debug_callback;

    virtual VkSurfaceKHR createSurface() = 0;
    Result loadDeviceSymbols();
    Result loadInstanceSymbols();

    bool validateExtensions(
        const std::vector<const char *> &requiredExtensions,
        const std::vector<VkExtensionProperties> &availableExtensions);
};

} // namespace Tobi