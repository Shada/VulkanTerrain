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

  protected:
    Platform();

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
    static Platform &getInstance();

    virtual Result initialize() = 0;

    virtual Result initWindow() = 0;
};

} // namespace Tobi