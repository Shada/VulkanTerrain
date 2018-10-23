#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "../WindowXcb.hpp"
#include "VulkanDepthBuffer.hpp"
#include "../VulkanRenderPass.hpp"
#include "../VulkanSwapChain.hpp"
#include "../EventDispatcher.hpp"

namespace Tobi
{

class VulkanFrameBuffers
{
  public:
    VulkanFrameBuffers(
        std::shared_ptr<WindowXcb> window,
        std::shared_ptr<VulkanDepthBuffer> depthBuffer,
        std::shared_ptr<VulkanRenderPass> renderPass,
        std::shared_ptr<VulkanSwapChain> swapChain,
        bool includeDepth);
    VulkanFrameBuffers(const VulkanFrameBuffers &) = delete;
    VulkanFrameBuffers(VulkanFrameBuffers &&) = default;
    VulkanFrameBuffers &operator=(const VulkanFrameBuffers &) & = delete;
    VulkanFrameBuffers &operator=(VulkanFrameBuffers &&) & = default;
    ~VulkanFrameBuffers();

    const VkFramebuffer &getCurrentFrameBuffer() { return frameBuffers[swapChain->getCurrentBuffer()]; }

    void clean();
    void create();

  private:
    void initFrameBuffers();

    std::shared_ptr<WindowXcb> window;
    std::shared_ptr<VulkanDepthBuffer> depthBuffer;
    std::shared_ptr<VulkanRenderPass> renderPass;
    std::shared_ptr<VulkanSwapChain> swapChain;
    bool includeDepth;

    std::vector<VkFramebuffer> frameBuffers;
};
} // namespace Tobi