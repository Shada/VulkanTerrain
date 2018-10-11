#include "VulkanCommandBuffer.hpp"

#include "util.hpp"

namespace Tobi
{

VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<WindowXcb> window, std::shared_ptr<VulkanCommandPool> commandPool)
    : window(window),
      commandPool(commandPool),
      commandBufferCount(1),
      commandBuffer(nullptr)
{
    initCommandBuffer();
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    if (commandBuffer)
    {
        VkCommandBuffer commandBuffers[1] = {commandBuffer};
        vkFreeCommandBuffers(window->getDevice(), commandPool->getCommandPool(), commandBufferCount, commandBuffers);
    }
}

void VulkanCommandBuffer::initCommandBuffer()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool->getCommandPool();
    commandBufferAllocateInfo.commandBufferCount = commandBufferCount;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    result = vkAllocateCommandBuffers(window->getDevice(), &commandBufferAllocateInfo, &commandBuffer);
    assert(result == VK_SUCCESS);
}

void VulkanCommandBuffer::executeBeginCommandBuffer()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    assert(result == VK_SUCCESS);
}

void VulkanCommandBuffer::executeEndCommandBuffer()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;

    result = vkEndCommandBuffer(commandBuffer);
    assert(result == VK_SUCCESS);
}

} // namespace Tobi