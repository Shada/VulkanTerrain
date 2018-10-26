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

#include "CommandBufferManager.hpp"

namespace Tobi
{

CommandBufferManager::CommandBufferManager(
    VkDevice device,
    VkCommandBufferLevel bufferLevel,
    uint32_t queueFamilyIndex)
    : device(device),
      commandPool(VK_NULL_HANDLE),
      commandBuffers(std::vector<VkCommandBuffer>()),
      commandBufferLevel(bufferLevel),
      activeCommandBufferCount(0)
{
    LOGI("CONSTRUCTING CommandBufferManager\n");
    VkCommandPoolCreateInfo commandPoolCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
}

CommandBufferManager::~CommandBufferManager()
{
    LOGI("DECONSTRUCTING CommandBufferManager\n");
    if (!commandBuffers.empty())
        vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
    vkDestroyCommandPool(device, commandPool, nullptr);
}

void CommandBufferManager::beginFrame()
{
    activeCommandBufferCount = 0;
    vkResetCommandPool(device, commandPool, 0);
}

VkCommandBuffer CommandBufferManager::requestCommandBuffer()
{
    // Either we recycle a previously allocated command buffer, or create a new
    // one.
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    if (activeCommandBufferCount < commandBuffers.size())
    {
        commandBuffer = commandBuffers[activeCommandBufferCount++];
    }
    else
    {
        VkCommandBufferAllocateInfo commandBufferCreateInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        commandBufferCreateInfo.commandPool = commandPool;
        commandBufferCreateInfo.level = commandBufferLevel;
        commandBufferCreateInfo.commandBufferCount = 1;
        VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferCreateInfo, &commandBuffer));
        commandBuffers.push_back(commandBuffer);

        activeCommandBufferCount++;
    }

    return commandBuffer;
}

} // namespace Tobi