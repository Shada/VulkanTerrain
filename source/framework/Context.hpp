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

namespace Tobi
{
class Platform;
class PerFrame;

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

    Result initialize();

  private:
    std::unique_ptr<Platform> platform;

    std::vector<BackBuffer> backBuffers;
    VkRenderPass renderPass;

    // TODO: move to pipeline class
    VkPipelineCache pipelineCache;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<std::unique_ptr<PerFrame>> perFrame;

    void terminateBackBuffers();

    Result onPlatformUpdate();

    void updateSwapChain();
    void initRenderPass(VkFormat format);
    void initPipeline();

    void waitIdle();

    VkShaderModule loadShaderModule(VkDevice device, const char *pPath);
};

} // namespace Tobi