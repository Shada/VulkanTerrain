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

#include "Common.hpp"
#include "TobiStatus.hpp"
#include "ShaderDataBlock.hpp"
#include "ShaderDataBlock.hpp"
#include "buffers/Buffer.hpp"
#include "model/ModelManager.hpp"
#include "model/ObjectManager.hpp"
#include "../game/Camera.hpp"
#include "../game/KeyState.hpp"
#include "../platform/SwapChainDimensions.hpp"

namespace Tobi
{
class Platform;
class PerFrame;
class VertexBufferManager;
class IndexBufferManager;
class UniformBufferManager;
class FenceManager;

struct BackBuffer
{
    // We get this image from the platform. Its memory is bound to the display or
    // window.
    VkImage image;

    // We need an image view to be able to access the image as a framebuffer.
    VkImageView view;

    VkFramebuffer frameBuffer;
};

/// @brief The Context is the primary way for samples to interact
/// with the swapchain and get rendered images to screen.
class Context
{
  public:
    Context();
    Context(const Context &) = delete;
    Context(Context &&) = delete;
    Context &operator=(const Context &) & = delete;
    Context &operator=(Context &&) & = delete;
    ~Context();

    Result initialize();

    Result update(float time);
    Result render();

    uint32_t loadModel(const char *filename);

    Result acquireNextImage(uint32_t &swapChainIndex);

    Result presentImage(uint32_t index);

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
    VkSemaphore beginFrame(uint32_t index, VkSemaphore acquireSemaphore);

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
    void submitSwapChain(VkCommandBuffer cmdBuffer);

    const VkCommandBuffer &requestPrimaryCommandBuffer() const;

    double getCurrentTime();

    TobiStatus getWindowStatus();

    const SwapChainDimensions &getSwapChainDimensions() const;

    const auto &getBackBuffer(uint32_t swapChainIndex) const { return backBuffers[swapChainIndex]; }

  private:
    std::shared_ptr<Platform> platform;

    std::vector<BackBuffer> backBuffers;

    VkRenderPass renderPass;

    VkFormat depthBufferFormat;
    // Memory for the depth buffer.
    VkDeviceMemory depthBufferMemory;
    // Image for the depth buffer.
    VkImage depthBufferImage;
    // Image view for the depth buffer.
    VkImageView depthBufferView;

    // TODO: move to pipeline class
    VkPipelineCache pipelineCache;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<std::unique_ptr<PerFrame>> perFrame;

    std::shared_ptr<VertexBufferManager> vertexBufferManager;
    std::shared_ptr<IndexBufferManager> indexBufferManager;
    std::shared_ptr<UniformBufferManager> uniformBufferManager;

    std::unique_ptr<ModelManager> modelManager;
    std::unique_ptr<ObjectManager> objectManager;

    std::shared_ptr<Camera> camera;

    std::shared_ptr<KeyStates> keyStates;

    ShaderDataBlock shaderDataBlock;

    // change to have these in a container?
    uint32_t triangleId;
    uint32_t cubeId;
    uint32_t spiderId;
    uint32_t cube2Id;

    uint32_t swapChainIndex;

    void terminateBackBuffers();

    Result onPlatformUpdate();

    void updateSwapChain();
    void initRenderPass(VkFormat format);
    void initPipeline();

    std::shared_ptr<FenceManager> &getFenceManager();

    /// @brief Gets the acquire semaphore for the swapchain.
    /// Used by the platform internally.
    /// @returns Semaphore.
    const VkSemaphore &getSwapChainAcquireSemaphore() const;

    /// @brief Gets the release semaphore for the swapchain.
    /// Used by the platform internally.
    /// @returns Semaphore.
    const VkSemaphore &getSwapChainReleaseSemaphore() const;

    void waitIdle();
    void submitCommandBuffer(VkCommandBuffer commandBuffer, VkSemaphore acquireSemaphore, VkSemaphore releaseSemaphore);

    VkShaderModule loadShaderModule(VkDevice device, const char *pPath);
    void initDepthBuffer(uint32_t width, uint32_t height);
};

} // namespace Tobi