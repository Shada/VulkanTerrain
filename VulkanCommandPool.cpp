#include "VulkanCommandPool.hpp"

#include "util.hpp"

namespace Tobi
{

VulkanCommandPool::VulkanCommandPool(std::shared_ptr<WindowXcb> window)
    : window(window)
{
    initCommandPool();
}

VulkanCommandPool::~VulkanCommandPool()
{
    if (commandPool)
        vkDestroyCommandPool(window->getDevice(), commandPool, nullptr);
}

void VulkanCommandPool::initCommandPool()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.queueFamilyIndex = window->getGraphicsQueueIndex();
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    result = vkCreateCommandPool(window->getDevice(), &commandPoolCreateInfo, nullptr, &commandPool);
    assert(result == VK_SUCCESS);
}

} // namespace Tobi