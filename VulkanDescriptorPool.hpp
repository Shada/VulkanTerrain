#pragma once

#include <vulkan/vulkan.hpp>

#include "WindowXcb.hpp"

namespace Tobi
{

class VulkanDescriptorPool
{
public:
  VulkanDescriptorPool(std::shared_ptr<WindowXcb> window, bool useTexture);
  VulkanDescriptorPool(const VulkanDescriptorPool &) = delete;
  VulkanDescriptorPool(VulkanDescriptorPool &&) = default;
  VulkanDescriptorPool &operator=(const VulkanDescriptorPool &) & = delete;
  VulkanDescriptorPool &operator=(VulkanDescriptorPool &&) & = default;
  ~VulkanDescriptorPool();

  const VkDescriptorPool &getDescriptorPool() { return descriptorPool; }

private:
  void initDescriptorPool();

  std::shared_ptr<WindowXcb> window;
  bool useTexture;

  VkDescriptorPool descriptorPool;
};

} // namespace Tobi