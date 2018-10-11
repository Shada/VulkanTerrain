#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanCommandPool.hpp"

namespace Tobi
{

class VulkanCommandBuffer
{
  public:
    VulkanCommandBuffer(std::shared_ptr<WindowXcb> window, std::shared_ptr<VulkanCommandPool> commandPool);
    VulkanCommandBuffer(const VulkanCommandBuffer &) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer &&) = default;
    VulkanCommandBuffer &operator=(const VulkanCommandBuffer &) & = delete;
    VulkanCommandBuffer &operator=(VulkanCommandBuffer &&) & = default;
    ~VulkanCommandBuffer();

    void executeBeginCommandBuffer();
    void executeEndCommandBuffer();

    const VkCommandBuffer &getCommandBuffer() { return commandBuffer; }

  private:
    void initCommandBuffer();

    std::shared_ptr<WindowXcb> window;
    std::shared_ptr<VulkanCommandPool> commandPool;

    uint32_t commandBufferCount;
    VkCommandBuffer commandBuffer;
};

} // namespace Tobi