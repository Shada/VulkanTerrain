#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>

#include "../WindowXcb.hpp"

namespace Tobi
{

class VulkanShaderProgram
{
public:
  VulkanShaderProgram(std::shared_ptr<WindowXcb> window);
  VulkanShaderProgram(const VulkanShaderProgram &) = delete;
  VulkanShaderProgram(VulkanShaderProgram &&) = default;
  VulkanShaderProgram &operator=(const VulkanShaderProgram &) & = delete;
  VulkanShaderProgram &operator=(VulkanShaderProgram &&) & = default;
  ~VulkanShaderProgram();

  const std::vector<VkPipelineShaderStageCreateInfo> &getShaderStages() { return shaderStages; }

private:
  void initShaders();

  std::shared_ptr<WindowXcb> window;

  std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
};
} // namespace Tobi