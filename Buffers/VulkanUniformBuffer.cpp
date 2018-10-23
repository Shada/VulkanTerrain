#include "VulkanUniformBuffer.hpp"

#include <cstring>

#include "AssertTypeDefine.hpp"

namespace Tobi
{
VulkanUniformBuffer::VulkanUniformBuffer(std::shared_ptr<WindowXcb> window, void *data, uint32_t dataSize)
    : window(window),
      data(data),
      dataSize(dataSize),
      buffer(nullptr),
      memory(nullptr)
{
    initUniformBuffer();
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
    if (buffer)
    {
        vkDestroyBuffer(window->getDevice(), buffer, nullptr);
        vkFreeMemory(window->getDevice(), memory, nullptr);
    }
}

void VulkanUniformBuffer::initUniformBuffer()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;
    bool U_ASSERT_ONLY pass = true;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.size = dataSize;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    result = vkCreateBuffer(window->getDevice(), &bufferCreateInfo, nullptr, &buffer);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(window->getDevice(), buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocationInfo = {};
    memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocationInfo.pNext = nullptr;
    memoryAllocationInfo.memoryTypeIndex = 0;
    memoryAllocationInfo.allocationSize = memoryRequirements.size;

    pass = window->memoryTypeFromProperties(memoryRequirements.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            &memoryAllocationInfo.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    result = vkAllocateMemory(window->getDevice(), &memoryAllocationInfo, nullptr, &(memory));
    assert(result == VK_SUCCESS);

    uint8_t *pData;
    result = vkMapMemory(window->getDevice(), memory, 0, memoryRequirements.size, 0, (void **)&pData);
    assert(result == VK_SUCCESS);

    memcpy(pData, data, dataSize);

    vkUnmapMemory(window->getDevice(), memory);

    result = vkBindBufferMemory(window->getDevice(), buffer, memory, 0);
    assert(result == VK_SUCCESS);

    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = dataSize;
}

} // namespace Tobi