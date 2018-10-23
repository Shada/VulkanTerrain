#pragma once

#include <vulkan/vulkan.h>

#include "../WindowXcb.hpp"

namespace Tobi
{

class VulkanPipelineCache
{
  public:
    VulkanPipelineCache(std::shared_ptr<WindowXcb> window);
    VulkanPipelineCache(const VulkanPipelineCache &) = delete;
    VulkanPipelineCache(VulkanPipelineCache &&) = default;
    VulkanPipelineCache &operator=(const VulkanPipelineCache &) & = delete;
    VulkanPipelineCache &operator=(VulkanPipelineCache &&) & = default;
    ~VulkanPipelineCache();

    const VkPipelineCache &getPipelineCache() { return pipelineCache; }

  private:
    void initPipelineCache();

    std::shared_ptr<WindowXcb> window;

    VkPipelineCache pipelineCache;
};

} // namespace Tobi