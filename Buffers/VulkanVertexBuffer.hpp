#pragma once
#include <unistd.h>
#include <vulkan/vulkan.h>

#include "../WindowXcb.hpp"

namespace Tobi
{

class VulkanVertexBuffer
{
public:
  VulkanVertexBuffer(std::shared_ptr<WindowXcb> window, const void *data, uint32_t dataSize, uint32_t stride, bool useTexture);
  VulkanVertexBuffer(const VulkanVertexBuffer &) = delete;
  VulkanVertexBuffer(VulkanVertexBuffer &&) = default;
  VulkanVertexBuffer &operator=(const VulkanVertexBuffer &) & = delete;
  VulkanVertexBuffer &operator=(VulkanVertexBuffer &&) & = default;
  ~VulkanVertexBuffer();

  const VkBuffer &getBuffer() { return buffer; }
  const VkVertexInputBindingDescription &getVertexInputBinding() { return vertexInputBinding; }
  const VkVertexInputAttributeDescription *getVertexInputAttributeDescription() { return vertexInputAttributes; }

private:
  void initVertexBuffer();

  std::shared_ptr<WindowXcb> window;
  const void *data;
  uint32_t dataSize;
  uint32_t stride;
  bool useTexture;

  VkBuffer buffer;
  VkDeviceMemory memory;
  VkDescriptorBufferInfo bufferInfo;
  VkVertexInputBindingDescription vertexInputBinding;
  VkVertexInputAttributeDescription vertexInputAttributes[2];
};

} // namespace Tobi