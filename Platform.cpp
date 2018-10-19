#include "Platform.hpp"

namespace Tobi
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
    uint64_t object, size_t location, int32_t messageCode,
    const char *layerPrefix, const char *message, void *userData)
{
    auto *platform = static_cast<Platform *>(userData);
    auto callback = platform->getExternalDebugCallback();

    if (callback)
    {
        return callback(flags, type, object, location, messageCode, layerPrefix, message,
                        platform->getExternalDebugCallbackUserData());
    }
    else
    {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            LOGE("Validation Layer: Error: %s: %s\n", layerPrefix, message);
        }
        else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            LOGE("Validation Layer: Warning: %s: %s\n", layerPrefix, message);
        }
        else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            LOGI("Validation Layer: Performance warning: %s: %s\n", layerPrefix, message);
        }
        else
        {
            LOGI("Validation Layer: Information: %s: %s\n", layerPrefix, message);
        }
        return VK_FALSE;
    }
}

Platform::~Platform()
{
    terminate(); // destroy all vulkan stuff
}

void Platform::terminate()
{
    // Don't release anything until the GPU is completely idle.
    if (device)
        vkDeviceWaitIdle(device);
}

} // namespace Tobi