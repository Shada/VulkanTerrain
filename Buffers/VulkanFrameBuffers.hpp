#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "../WindowXcb.hpp"
#include "VulkanDepthBuffer.hpp"
#include "../VulkanRenderPass.hpp"
#include "../VulkanSwapChain.hpp"
#include "../EventDispatcher.hpp"

namespace Tobi
{

class VulkanFrameBuffers
    : public Dispatcher<ResizeWindowEvent>::Listener
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

    virtual void onEvent(ResizeWindowEvent &event, Dispatcher<ResizeWindowEvent> &sender)
    {
        // recreate swapchain here.
        std::cout << "FrameBuffer: onEvent ResizeWindowEvent " << event.width << "x" << event.height << std::endl;
       // resizeSwapChain();
    }

  private:
    void initFrameBuffers();

    void resizeSwapChain();

    std::shared_ptr<WindowXcb> window;
    std::shared_ptr<VulkanDepthBuffer> depthBuffer;
    std::shared_ptr<VulkanRenderPass> renderPass;
    std::shared_ptr<VulkanSwapChain> swapChain;
    bool includeDepth;

    std::vector<VkFramebuffer> frameBuffers;
};
} // namespace Tobi