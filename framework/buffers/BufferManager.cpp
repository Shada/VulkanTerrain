#include "BufferManager.hpp"

namespace Tobi
{

BufferManager::BufferManager(std::shared_ptr<Platform> platform)
    : platform(platform),
      buffers(std::vector<Buffer>())
{
}

BufferManager::~BufferManager()
{
    for (auto &buffer : buffers)
    {
        if (buffer.buffer)
        {
            vkDestroyBuffer(platform->getDevice(), buffer.buffer, nullptr);
            vkFreeMemory(platform->getDevice(), buffer.memory, nullptr);
        }
    }
}

const Buffer &BufferManager::createBuffer(
    const void *data,
    const uint32_t dataSize,
    VkFlags usageFlags)
{
    auto result = VK_SUCCESS;

    Buffer buffer;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.usage = usageFlags;
    bufferCreateInfo.size = dataSize;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    VK_CHECK(vkCreateBuffer(platform->getDevice(), &bufferCreateInfo, nullptr, &buffer.buffer));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(platform->getDevice(), buffer.buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocationInfo = {};
    memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocationInfo.pNext = nullptr;
    memoryAllocationInfo.memoryTypeIndex = 0;
    memoryAllocationInfo.allocationSize = memoryRequirements.size;

    memoryAllocationInfo.memoryTypeIndex = platform->findMemoryTypeFromRequirements(
        memoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VK_CHECK(vkAllocateMemory(platform->getDevice(), &memoryAllocationInfo, nullptr, &(buffer.memory)));

    uint8_t *pData;
    VK_CHECK(vkMapMemory(platform->getDevice(), buffer.memory, 0, memoryRequirements.size, 0, (void **)&pData));

    memcpy(pData, data, dataSize);

    vkUnmapMemory(platform->getDevice(), buffer.memory);

    VK_CHECK(vkBindBufferMemory(platform->getDevice(), buffer.buffer, buffer.memory, 0));

    buffer.bufferInfo.buffer = buffer.buffer;
    buffer.bufferInfo.offset = 0;
    buffer.bufferInfo.range = dataSize;

    buffers.push_back(buffer);

    return buffers.back();
}

} // namespace Tobi