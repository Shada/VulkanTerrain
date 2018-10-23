#pragma once

#include <vulkan/vulkan.h>

#include "WindowXcb.hpp"
#include "Buffers/VulkanDepthBuffer.hpp"

namespace Tobi
{

class VulkanRenderPass
{
  public:
    VulkanRenderPass(std::shared_ptr<WindowXcb> window,
                     std::shared_ptr<VulkanDepthBuffer> depthBuffer,
                     bool includeDepth,
                     bool clear = true,
                     VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    VulkanRenderPass(const VulkanRenderPass &) = delete;
    VulkanRenderPass(VulkanRenderPass &&) = default;
    VulkanRenderPass &operator=(const VulkanRenderPass &) & = delete;
    VulkanRenderPass &operator=(VulkanRenderPass &&) & = default;
    ~VulkanRenderPass();

    void clean();
    void create();

    const VkRenderPass &getRenderPass() { return renderPass; }

  private:
    void initRenderPass();

    std::shared_ptr<WindowXcb> window;
    std::shared_ptr<VulkanDepthBuffer> depthBuffer;
    bool includeDepth;
    bool clear;
    VkImageLayout imageLayout;

    VkRenderPass renderPass;
};

} // namespace Tobi