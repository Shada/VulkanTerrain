#pragma once

namespace Tobi
{
struct Buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDescriptorBufferInfo bufferInfo;
};
} // namespace Tobi