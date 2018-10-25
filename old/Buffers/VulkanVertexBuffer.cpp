#include "VulkanVertexBuffer.hpp"

#include <cstring>
#include <cassert>

#include "AssertTypeDefine.hpp"

namespace Tobi
{

VulkanVertexBuffer::VulkanVertexBuffer(
    std::shared_ptr<WindowXcb> window,
    const void *data,
    uint32_t dataSize,
    uint32_t stride,
    bool useTexture)
    : window(window),
      data(data),
      dataSize(dataSize),
      stride(stride),
      useTexture(useTexture),
      buffer(nullptr),
      memory(nullptr)
{
    initVertexBuffer();
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    if (buffer)
    {
        vkDestroyBuffer(window->getDevice(), buffer, nullptr);
        vkFreeMemory(window->getDevice(), memory, nullptr);
    }
}

void VulkanVertexBuffer::initVertexBuffer()
{
    auto result = VK_SUCCESS;
    auto pass = true;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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

    vertexInputBinding.binding = 0;
    vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputBinding.stride = stride;

    vertexInputAttributes[0].binding = 0;
    vertexInputAttributes[0].location = 0;
    vertexInputAttributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributes[0].offset = 0;
    vertexInputAttributes[1].binding = 0;
    vertexInputAttributes[1].location = 1;
    vertexInputAttributes[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributes[1].offset = 16;
}

} // namespace Tobi