#include <memory>
#include <iostream>

#include "platform/xcb/PlatformXcb.hpp"
#include "framework/model/Model.hpp"
#include "framework/buffers/VertexBufferManager.hpp"

// TODO: change so that platform is within context, and is not returned.
// Only use the context to interact with vulkan platform

namespace Tobi
{

int run(std::shared_ptr<Platform>, std::shared_ptr<VertexBufferManager>);

int init()
{
    auto platform = std::make_shared<PlatformXcb>();

    auto model = std::make_shared<Model>();

    auto vertexBuffer = std::make_shared<VertexBufferManager>(platform);

    vertexBuffer->createBuffer(
        model->getVertexData(),
        model->getVertexDataSize());

    auto context = platform->getContext();

    context->updateSwapChain();

    run(platform, vertexBuffer);

    return 0;
}

void render(std::shared_ptr<Platform> platform, std::shared_ptr<VertexBufferManager> vertexBuffer, uint32_t swapChainIndex, float time)
{
    auto context = platform->getContext();
    // Render to this backbuffer.
    auto &backBuffer = context->getBackBuffer(swapChainIndex);

    // Request a fresh command buffer.
    VkCommandBuffer cmd = context->requestPrimaryCommandBuffer();

    // We will only submit this once before it's recycled.
    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    // Set clear color values.
    VkClearValue clearValue;
    clearValue.color.float32[0] = 0.1f;
    clearValue.color.float32[1] = 0.1f;
    clearValue.color.float32[2] = 0.2f;
    clearValue.color.float32[3] = 1.0f;

    // Begin the render pass.
    auto dim = platform->getSwapChainDimensions();
    VkRenderPassBeginInfo rpBegin = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpBegin.renderPass = context->getRenderPass();
    rpBegin.framebuffer = backBuffer.frameBuffer;
    rpBegin.renderArea.extent.width = dim.width;
    rpBegin.renderArea.extent.height = dim.height;
    rpBegin.clearValueCount = 1;
    rpBegin.pClearValues = &clearValue;
    // We will add draw commands in the same command buffer.
    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline.
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, context->getPipeline());

    // Set up dynamic state.
    // Viewport
    VkViewport vp = {0};
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.width = float(dim.width);
    vp.height = float(dim.height);
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &vp);

    // Scissor box
    VkRect2D scissor;
    memset(&scissor, 0, sizeof(scissor));
    scissor.extent.width = dim.width;
    scissor.extent.height = dim.height;
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // Bind vertex buffer.
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer->getBuffer(0).buffer, &offset);

    // Draw three vertices with one instance.
    vkCmdDraw(cmd, 3, 1, 0, 0);

    // Complete render pass.
    vkCmdEndRenderPass(cmd);

    // Complete the command buffer.
    VK_CHECK(vkEndCommandBuffer(cmd));

    // Submit it to the queue.
    context->submitSwapchain(cmd);
}

int run(std::shared_ptr<Platform> platform, std::shared_ptr<VertexBufferManager> vertexBuffer)
{
    auto frameCount = static_cast<uint32_t>(0);
    auto startTime = OS::getCurrentTime();

    auto maxFrameCount = static_cast<uint32_t>(100);
    auto useMaxFrameCount = true;
    auto context = platform->getContext();

    while (platform->getWindowStatus() == Platform::STATUS_RUNNING)
    {
        uint32_t swapChainIndex;
        auto result = platform->acquireNextImage(swapChainIndex);

        while (result == RESULT_ERROR_OUTDATED_SWAPCHAIN)
        {
            result = platform->acquireNextImage(swapChainIndex);
            auto context = platform->getContext();
            context->updateSwapChain();
        }

        if (FAILED(result))
        {
            LOGE("Unrecoverable swapchain error.\n");
            break;
        }

        render(platform, vertexBuffer, swapChainIndex, 0.0166f);
        result = platform->presentImage(swapChainIndex);

        // Handle Outdated error in acquire.
        if (FAILED(result) && result != RESULT_ERROR_OUTDATED_SWAPCHAIN)
            break;

        frameCount++;
        if (frameCount == 100)
        {
            auto endTime = OS::getCurrentTime();
            LOGI("FPS: %.3f\n", frameCount / (endTime - startTime));
            frameCount = 0;
            startTime = endTime;
        }
        if (useMaxFrameCount && (--maxFrameCount == 0))
            break;
    }
    platform->waitDeviceIdle();
    return 0;
}

} // namespace Tobi

int main()
{
    Tobi::init();

    //Tobi::run();

    return 0;
}