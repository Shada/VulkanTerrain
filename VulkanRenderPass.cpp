#include "VulkanRenderPass.hpp"

#include <cassert>

#include "AssertTypeDefine.hpp"
#include "util.hpp"

namespace Tobi
{

VulkanRenderPass::VulkanRenderPass(std::shared_ptr<WindowXcb> window,
                                   std::shared_ptr<VulkanDepthBuffer> depthBuffer,
                                   bool includeDepth,
                                   bool clear,
                                   VkImageLayout finalLayout)
    : window(window),
      depthBuffer(depthBuffer),
      includeDepth(includeDepth),
      clear(clear),
      imageLayout(finalLayout)
{
    initRenderPass();
}

VulkanRenderPass::~VulkanRenderPass()
{
    clean();
}

void VulkanRenderPass::initRenderPass()
{
    create();
}

void VulkanRenderPass::create()
{
    auto U_ASSERT_ONLY result = VK_SUCCESS;
    // Need imageViewAttachments for render target and depth buffer
    VkAttachmentDescription imageViewAttachments[2];
    imageViewAttachments[0].format = window->getSurfaceFormat();
    imageViewAttachments[0].samples = NUM_SAMPLES;
    imageViewAttachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    imageViewAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    imageViewAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    imageViewAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    imageViewAttachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageViewAttachments[0].finalLayout = imageLayout;
    imageViewAttachments[0].flags = 0;

    if (includeDepth)
    {
        imageViewAttachments[1].format = depthBuffer->getFormat();
        imageViewAttachments[1].samples = NUM_SAMPLES;
        imageViewAttachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        imageViewAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        imageViewAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        imageViewAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        imageViewAttachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageViewAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        imageViewAttachments[1].flags = 0;
    }

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = includeDepth ? &depthAttachmentReference : nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.attachmentCount = includeDepth ? 2 : 1;
    renderPassCreateInfo.pAttachments = imageViewAttachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = nullptr;

    result = vkCreateRenderPass(window->getDevice(), &renderPassCreateInfo, nullptr, &renderPass);
    assert(result == VK_SUCCESS);
}

void VulkanRenderPass::clean()
{

    if (renderPass)
    {
        vkDestroyRenderPass(window->getDevice(), renderPass, nullptr);
    }
}

} // namespace Tobi