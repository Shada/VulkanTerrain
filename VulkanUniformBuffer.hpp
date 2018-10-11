#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "WindowXcb.hpp"

namespace Tobi
{

class VulkanUniformBuffer
{
  public:
    VulkanUniformBuffer(std::shared_ptr<WindowXcb> window, void *data, uint32_t dataSize);
    VulkanUniformBuffer(const VulkanUniformBuffer &) = delete;
    VulkanUniformBuffer(VulkanUniformBuffer &&) = default;
    VulkanUniformBuffer &operator=(const VulkanUniformBuffer &) & = delete;
    VulkanUniformBuffer &operator=(VulkanUniformBuffer &&) & = default;
    ~VulkanUniformBuffer();

    const VkDescriptorBufferInfo &getBufferInfo() { return bufferInfo; }

  private:
    void initUniformBuffer();

    std::shared_ptr<WindowXcb> window;
    void *data;
    uint32_t dataSize;

    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDescriptorBufferInfo bufferInfo;
};

} // namespace Tobi