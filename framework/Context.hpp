/* Copyright (c) 2016-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <memory>
#include <vector>

#include "CommandBufferManager.hpp"
#include "FenceManager.hpp"
#include "Common.hpp"

#include "../platform/AssetManager.hpp"

namespace Tobi
{
class Platform;

struct BackBuffer
{
    // We get this image from the platform. Its memory is bound to the display or
    // window.
    VkImage image;

    // We need an image view to be able to access the image as a framebuffer.
    VkImageView view;

    // The actual frameBuffer.
    VkFramebuffer frameBuffer;
};

/// @brief The Context is the primary way for samples to interact
/// with the swapchain and get rendered images to screen.
class Context
{
  public:
    Context();
    ~Context();

    void terminate();
    void terminateBackBuffers();

    /// @brief Called by the platform internally when platform either initializes
    /// itself
    /// or the swapchain has been recreated.
    ///
    /// @param pPlatform The underlying Vulkan platform.
    /// @returns Error code
    Result onPlatformUpdate(Platform *pPlatform);

    /// @brief Requests a reset primary command buffer.
    ///
    /// The lifetime of this command buffer is only for the current frame.
    /// It must be submitted in the same frame that the application obtains the
    /// command buffer.
    ///
    /// @returns A reset primary command buffer
    VkCommandBuffer requestPrimaryCommandBuffer()
    {
        return perFrame[swapchainIndex]->commandManager->requestCommandBuffer();
    }

    /// @brief Requests a reset secondary command buffer, suitable for rendering
    /// multithreaded.
    ///
    /// The lifetime of this command buffer is only for the current frame.
    /// It must be submitted in the same frame that the application obtains the
    /// command buffer.
    ///
    /// @param threadIndex The worker thread index in range [0, N) which will be
    /// rendering using this secondary buffer.
    /// It is a race condition for two threads to use a command buffer which was
    /// obtained from the
    /// same threadIndex.
    /// In order to use secondary command buffers, the application must call @ref
    /// setRenderingThreadCount first.
    ///
    /// @returns A reset secondary command buffer
    VkCommandBuffer requestSecondaryCommandBuffer(uint32_t threadIndex)
    {
        return perFrame[swapchainIndex]->secondaryCommandManagers[threadIndex]->requestCommandBuffer();
    }

    /// @brief Submit a command buffer to the queue.
    /// @param cmdBuffer The commandbuffer to submit.
    void submit(VkCommandBuffer cmdBuffer);

    /// @brief Submit a command buffer to the queue which renders to the swapchain
    /// image.
    ///
    /// The difference between this and @ref submit is that extra semaphores might
    /// be added to
    /// the `vkQueueSubmit` call depending on what was passed in to @ref
    /// beginFrame by the platform.
    ///
    /// @param cmdBuffer The commandbuffer to submit.
    void submitSwapchain(VkCommandBuffer cmdBuffer);

    /// @brief Called by the platform, begins a frame
    ///
    /// @param index The swapchain index which will be rendered into this frame.
    ///
    /// @param acquireSemaphore When submitting command buffers using @ref
    /// submitSwapchain,
    /// Use the acquireSemaphore as a wait semaphore in `vkQueueSubmit`
    /// to wait for the swapchain to become ready before rendering begins on GPU.
    /// May be `VK_NULL_HANDLE` in case no waiting is required by the platform.
    ///
    /// @returns The old semaphore associated with this swapchain index.
    VkSemaphore beginFrame(uint32_t index, VkSemaphore acquireSemaphore)
    {
        swapchainIndex = index;
        perFrame[swapchainIndex]->beginFrame();
        return perFrame[swapchainIndex]->setSwapchainAcquireSemaphore(acquireSemaphore);
    }

    /// @brief Sets the number of worker threads which can use secondary command
    /// buffers.
    /// This call is blocking and will wait for all GPU work to complete before
    /// resizing.
    /// @param count The number of threads to support.
    void setRenderingThreadCount(uint32_t count)
    {
        vkQueueWaitIdle(queue);
        for (auto &pFrame : perFrame)
            pFrame->setSecondaryCommandManagersCount(count);
        renderingThreadCount = count;
    }

    /// @brief Gets the fence manager for the current swapchain image.
    /// Used by the platform internally.
    /// @returns FenceManager
    std::shared_ptr<FenceManager> &getFenceManager()
    {
        return perFrame[swapchainIndex]->fenceManager;
    }

    /// @brief Gets the acquire semaphore for the swapchain.
    /// Used by the platform internally.
    /// @returns Semaphore.
    const VkSemaphore &getSwapchainAcquireSemaphore() const
    {
        return perFrame[swapchainIndex]->swapchainAcquireSemaphore;
    }

    /// @brief Gets the release semaphore for the swapchain.
    /// Used by the platform internally.
    /// @returns Semaphore.
    const VkSemaphore &getSwapchainReleaseSemaphore() const
    {
        return perFrame[swapchainIndex]->swapchainReleaseSemaphore;
    }

    /// called when swapchain has been created/recreated
    void updateSwapChain();

    void initRenderPass(VkFormat format);
    void initPipeline();

  private:
    Platform *pPlatform;
    VkDevice device;
    VkQueue queue;
    uint32_t swapchainIndex;
    uint32_t renderingThreadCount;

    std::vector<BackBuffer> backBuffers;
    VkRenderPass renderPass;
    // TODO: move to pipeline class
    VkPipelineCache pipelineCache;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    //TODO: move to separate file
    struct PerFrame
    {
        PerFrame(VkDevice device, uint32_t graphicsQueueIndex);
        ~PerFrame();

        void beginFrame();
        VkSemaphore setSwapchainAcquireSemaphore(VkSemaphore acquireSemaphore);
        void setSwapchainReleaseSemaphore(VkSemaphore releaseSemaphore);
        void setSecondaryCommandManagersCount(uint32_t count);

        VkDevice device;
        std::shared_ptr<FenceManager> fenceManager;
        std::unique_ptr<CommandBufferManager> commandManager;
        std::vector<std::unique_ptr<CommandBufferManager>> secondaryCommandManagers;
        VkSemaphore swapchainAcquireSemaphore;
        VkSemaphore swapchainReleaseSemaphore;
        uint32_t queueIndex;
    };
    std::vector<std::unique_ptr<PerFrame>> perFrame;

    void submitCommandBuffer(VkCommandBuffer, VkSemaphore acquireSemaphore, VkSemaphore releaseSemaphore);
    void waitIdle();

    VkShaderModule loadShaderModule(VkDevice device, const char *pPath);
};

} // namespace Tobi