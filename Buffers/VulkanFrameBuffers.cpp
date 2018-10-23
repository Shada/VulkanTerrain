#include "VulkanFrameBuffers.hpp"

#include <cassert>

#include "AssertTypeDefine.hpp"

namespace Tobi
{

VulkanFrameBuffers::VulkanFrameBuffers(
    std::shared_ptr<WindowXcb> window,
    std::shared_ptr<VulkanDepthBuffer> depthBuffer,
    std::shared_ptr<VulkanRenderPass> renderPass,
    std::shared_ptr<VulkanSwapChain> swapChain,
    bool includeDepth)
    : window(window),
      depthBuffer(depthBuffer),
      renderPass(renderPass),
      swapChain(swapChain),
      includeDepth(includeDepth),
      frameBuffers(std::vector<VkFramebuffer>())
{
    initFrameBuffers();
}

VulkanFrameBuffers::~VulkanFrameBuffers()
{
    clean();
}

void VulkanFrameBuffers::initFrameBuffers()
{
    create();
}

void VulkanFrameBuffers::clean()
{
    for (auto &frameBuffer : frameBuffers)
    {
        if (frameBuffer)
        {
            vkDestroyFramebuffer(window->getDevice(), frameBuffer, nullptr);
        }
    }
}

void VulkanFrameBuffers::create()
{
    auto U_ASSERT_ONLY result = VK_SUCCESS;
    VkImageView imageViewAttachments[2];
    imageViewAttachments[1] = depthBuffer->getImageView();

    VkFramebufferCreateInfo frameBufferCreateInfo = {};

    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = nullptr;
    frameBufferCreateInfo.renderPass = renderPass->getRenderPass();
    frameBufferCreateInfo.attachmentCount = includeDepth ? 2 : 1;
    frameBufferCreateInfo.pAttachments = imageViewAttachments;
    frameBufferCreateInfo.width = window->getWidth();
    frameBufferCreateInfo.height = window->getHeight();
    frameBufferCreateInfo.layers = 1;

    frameBuffers.resize(swapChain->getSwapChainImageCount());

    for (uint32_t i = 0; i < swapChain->getSwapChainImageCount(); i++)
    {
        imageViewAttachments[0] = swapChain->getSwapChainBuffer(i).view;
        result = vkCreateFramebuffer(window->getDevice(), &frameBufferCreateInfo, nullptr, &frameBuffers[i]);
        assert(result == VK_SUCCESS);
    }
}

} // namespace Tobi