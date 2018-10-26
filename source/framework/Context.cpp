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

#include "Context.hpp"

#include "../platform/Platform.hpp"
#include "PerFrame.hpp"
#include "model/Vertex.hpp"

namespace Tobi
{

Context::Context()
    : platform(Platform::create()),
      pipelineCache(VK_NULL_HANDLE),
      perFrame(std::vector<std::unique_ptr<PerFrame>>())
{
    LOGI("CONSTRUCTING Context\n");
}

Context::~Context()
{
    LOGI("DECONSTRUCTING Context\n");
    if (pipelineCache)
    {
        auto device = platform->getDevice();
        vkDestroyPipelineCache(device, pipelineCache, nullptr);
        pipelineCache = VK_NULL_HANDLE;
    }
}

Result Context::initialize()
{
    LOGI("START INITIALIZING Context\n");
    auto result = platform->initialize();
    if (FAILED(result))
    {
        LOGE("Failed to initialize Platform\n");
        return RESULT_ERROR_GENERIC;
    }

    // attach context to the platform ?? does it need the context in any way?

    if (FAILED(onPlatformUpdate()))
    {
        LOGE("Failed to create per frame data\n");
        return RESULT_ERROR_GENERIC;
    }

    LOGI("FINISHED INITIALIZING Context\n");
    return RESULT_SUCCESS;
}

Result Context::onPlatformUpdate()
{
    auto device = platform->getDevice();

    waitIdle();

    // Initialize per-frame resources.
    // Every swapchain image has its own command pool and fence manager.
    // This makes it very easy to keep track of when we can reset command buffers
    // and such.
    perFrame.clear();
    for (uint32_t i = 0; i < platform->getSwapChainImageCount(); i++)
        perFrame.emplace_back(new PerFrame(device, platform->getGraphicsQueueFamilyIndex()));
    /*
    setRenderingThreadCount(renderingThreadCount);*/

    // Create a pipeline cache (although we'll only create one pipeline).
    VkPipelineCacheCreateInfo pipelineCacheInfo = {VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
    VK_CHECK(vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache));

    return RESULT_SUCCESS;
}

void Context::waitIdle()
{
    platform->waitIdle();
}

} // namespace Tobi
