#pragma once

namespace Tobi
{

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

} // namespace Tobi