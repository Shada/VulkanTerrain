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

#include <vector>

#include "Common.hpp"

namespace Tobi
{
/// @brief The FenceManager keeps track of fences which in turn are used to keep
/// track of GPU progress.
///
/// Whenever we submit work to the GPU, it is our responsibility to make sure
/// that the GPU is done
/// using our resources before we modify or delete them.
/// To implement this, we therefore use VkFences to keep track of all
/// vkQueueSubmits.
class FenceManager
{
  public:
    /// @brief Constructor
    /// @param device The Vulkan device
    FenceManager(VkDevice device);

    FenceManager(const FenceManager &) = delete;
    FenceManager(FenceManager &&) = delete;
    FenceManager &operator=(const FenceManager &) & = delete;
    FenceManager &operator=(FenceManager &&) & = delete;

    /// @brief Destructor
    ~FenceManager();

    /// @brief Begins the frame. Waits for GPU to trigger all outstanding fences.
    /// After begin frame returns, it is safe to reuse or delete resources which
    /// were used previously.
    ///
    /// We wait for fences which completes N frames earlier, so we do not stall,
    /// waiting
    /// for all GPU work to complete before this returns.
    void beginFrame();

    /// @brief Called internally by the FenceManager whenever submissions to GPU
    /// happens.
    VkFence requestClearedFence();

    /// @brief Gets the number of fences which are inFlight on the GPU.
    /// @returns The number of fences which can be waited for.
    uint32_t getActiveFenceCount() const
    {
        return activeFenceCount;
    }

    /// @brief Gets an array for the fences which are inFlight on the GPU.
    /// @returns Array of waitable fences. Call @ref getActiveFenceCount for the
    /// number of fences.
    VkFence *getActiveFences()
    {
        return fences.data();
    }

  private:
    VkDevice device;
    std::vector<VkFence> fences;
    uint32_t activeFenceCount;
};

} // namespace Tobi