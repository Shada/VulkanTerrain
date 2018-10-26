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

Platform::Platform()
{
    LOGI("CONTRUCTING Platform\n");
}

Platform::~Platform()
{
    LOGI("DECONTRUCTING Platform\n");
}

Result Platform::initVulkan(
    const SwapChainDimensions &swapChainDimensions,
    const std::vector<const char *> &requiredInstanceExtensions,
    const std::vector<const char *> &requiredDeviceExtensions)
{
    /* if (FAILED(initInstanceExtensions(requiredInstanceExtensions)))
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
*/
    return RESULT_SUCCESS;
}

} // namespace Tobi