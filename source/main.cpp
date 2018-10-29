#include <memory>
#include <iostream>

#include "framework/Context.hpp"

#include "framework/Status.hpp"

// TODO: change so that platform is within context, and is not returned.
// Only use the context to interact with vulkan platform

namespace Tobi
{

class ApplicationStart
{
  public:
    ApplicationStart()
    {
        context = std::make_shared<Context>();

        context->initialize();

        model_id = context->loadModel("modelname");
    }
    ~ApplicationStart() = default;

    int run()
    {
        auto frameCount = static_cast<uint32_t>(0);
        auto startTime = context->getCurrentTime();

        auto maxFrameCount = static_cast<uint32_t>(100);
        auto useMaxFrameCount = false;

        while (context->getWindowStatus() == Status::STATUS_RUNNING)
        {
            uint32_t swapChainIndex;
            auto result = context->acquireNextImage(swapChainIndex);

            if (FAILED(result))
            {
                LOGE("Unrecoverable swapchain error.\n");
                break;
            }
            render(swapChainIndex, 0.0166f);

            result = context->presentImage(swapChainIndex);

            if (FAILED(result) && result != RESULT_ERROR_OUTDATED_SWAPCHAIN)
                break;

            frameCount++;
            if (frameCount == 100)
            {
                auto endTime = context->getCurrentTime();
                LOGI("FPS: %.3f\n", frameCount / (endTime - startTime));
                frameCount = 0;
                startTime = endTime;
            }
            if (useMaxFrameCount && (--maxFrameCount == 0))
                break;
        }

        return 0;
    }

    void render(uint32_t swapChainIndex, float time)
    {
        // Render to this backbuffer.
        auto &backBuffer = context->getBackBuffer(swapChainIndex);

        // Request a fresh command buffer.
        auto cmd = context->requestPrimaryCommandBuffer();

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
        auto dim = context->getSwapChainDimensions();
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
        vkCmdBindVertexBuffers(cmd, 0, 1, &context->getVertexBuffer(0).buffer, &offset);

        // Draw three vertices with one instance.
        vkCmdDraw(cmd, 3, 1, 0, 0);

        // Complete render pass.
        vkCmdEndRenderPass(cmd);

        // Complete the command buffer.
        VK_CHECK(vkEndCommandBuffer(cmd));

        // Submit it to the queue.
        context->submitSwapChain(cmd);
    }

  private:
    uint32_t model_id;
    std::shared_ptr<Context> context;
};

} // namespace Tobi

int main()
{
    Tobi::ApplicationStart *applicationStart = new Tobi::ApplicationStart();

    applicationStart->run();

    delete applicationStart;
    applicationStart = nullptr;

    return 0;
}