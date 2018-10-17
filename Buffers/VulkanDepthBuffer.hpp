#pragma once

#include <vulkan/vulkan.hpp>

#include "../WindowXcb.hpp"
#include <memory>

namespace Tobi
{
class VulkanDepthBuffer
{
  public:
    VulkanDepthBuffer(std::shared_ptr<WindowXcb> window);
    VulkanDepthBuffer(const VulkanDepthBuffer &) = delete;
    VulkanDepthBuffer(VulkanDepthBuffer &&) = default;
    VulkanDepthBuffer &operator=(const VulkanDepthBuffer &) & = delete;
    VulkanDepthBuffer &operator=(VulkanDepthBuffer &&) & = default;
    ~VulkanDepthBuffer();

    void clean();
    void create();

    const VkImageView &getImageView() { return imageView; }

    const VkFormat &getFormat() { return format; }

  private:
    void initDepthBuffer();
    void initFormat();

    std::shared_ptr<WindowXcb> window;

    VkImage image;
    VkDeviceMemory memory;
    VkImageView imageView;
    VkFormat format;
    VkFormatProperties formatProperties;
};
} // namespace Tobi