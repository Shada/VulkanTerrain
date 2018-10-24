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

#include "FenceManager.hpp"

namespace Tobi
{

FenceManager::FenceManager(VkDevice device)
    : device(device),
      fences(std::vector<VkFence>()),
      activeFenceCount(0)
{
}

FenceManager::~FenceManager()
{
    beginFrame();
    for (auto &fence : fences)
        vkDestroyFence(device, fence, nullptr);
}

void FenceManager::beginFrame()
{
    // If we have outstanding fences for this swapchain image, wait for them to
    // complete first.
    // Normally, this doesn't really block at all,
    // since we're waiting for old frames to have been completed, but just in
    // case.
    if (activeFenceCount != 0)
    {
        vkWaitForFences(device, activeFenceCount, fences.data(), true, UINT64_MAX);
        vkResetFences(device, activeFenceCount, fences.data());
    }
    activeFenceCount = 0;
}

VkFence FenceManager::requestClearedFence()
{
    if (activeFenceCount < fences.size())
        return fences[activeFenceCount++];

    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
    fences.push_back(fence);
    activeFenceCount++;
    return fence;
}

} // namespace Tobi