#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "WindowXcb.hpp"

namespace Tobi
{
typedef struct TSwapChainBuffer
{
  VkImage image;
  VkImageView view;
} SwapChainBuffer;

class VulkanSwapChain
{
public:
  VulkanSwapChain(std::shared_ptr<WindowXcb> window);
  VulkanSwapChain(const VulkanSwapChain &) = delete;
  VulkanSwapChain(VulkanSwapChain &&) = default;
  VulkanSwapChain &operator=(const VulkanSwapChain &) & = delete;
  VulkanSwapChain &operator=(VulkanSwapChain &&) & = default;
  ~VulkanSwapChain();

  const VkSwapchainKHR &getSwapChain() { return swapChain; }
  uint32_t &getCurrentBuffer() { return currentBuffer; }
  const std::vector<SwapChainBuffer> &getSwapChainBuffers() { return swapChainBuffers; }
  const SwapChainBuffer &getSwapChainBuffer(uint32_t index) { return swapChainBuffers[index]; }
  const uint32_t &getSwapChainImageCount() { return swapChainImageCount; }

private:
  void initSwapChain(VkImageUsageFlags usageFlags =
                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

  std::shared_ptr<WindowXcb> window;

  VkSwapchainKHR swapChain;
  std::vector<SwapChainBuffer> swapChainBuffers;
  uint32_t swapChainImageCount;
  uint32_t currentBuffer;
};

} // namespace Tobi