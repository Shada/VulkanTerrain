#pragma once

#include <vulkan/vulkan.hpp>

namespace Tobi
{
struct Buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDescriptorBufferInfo bufferInfo;
};
} // namespace Tobi