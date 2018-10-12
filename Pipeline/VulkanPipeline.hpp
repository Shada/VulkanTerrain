#pragma once

#include <vulkan/vulkan.hpp>

#include "../WindowXcb.hpp"
#include "VulkanPipelineCache.hpp"
#include "../VulkanRenderPass.hpp"
#include "../Shader/VulkanShaderProgram.hpp"
#include "../Buffers/VulkanVertexBuffer.hpp"

namespace Tobi
{

class VulkanPipeline
{
  public:
    VulkanPipeline(
        std::shared_ptr<WindowXcb> window,
        std::shared_ptr<VulkanPipelineCache> pipelineCache,
        std::shared_ptr<VulkanRenderPass> renderPass,
        std::shared_ptr<VulkanShaderProgram> shaderProgram,
        std::shared_ptr<VulkanVertexBuffer> vertexBuffer,
        VkPipelineLayout &pipelineLayout,
        VkBool32 includeDepth,
        VkBool32 includeVertexInput = VK_TRUE);
    VulkanPipeline(const VulkanPipeline &) = delete;
    VulkanPipeline(VulkanPipeline &&) = default;
    VulkanPipeline &operator=(const VulkanPipeline &) & = delete;
    VulkanPipeline &operator=(VulkanPipeline &&) & = default;
    ~VulkanPipeline();

    const VkPipeline &getPipeline() { return pipeline; }

  private:
    void initPipeline();

    std::shared_ptr<WindowXcb> window;
    std::shared_ptr<VulkanPipelineCache> pipelineCache;
    std::shared_ptr<VulkanRenderPass> renderPass;
    std::shared_ptr<VulkanShaderProgram> shaderProgram;
    std::shared_ptr<VulkanVertexBuffer> vertexBuffer;
    VkPipelineLayout &pipelineLayout;
    VkBool32 includeDepth;
    VkBool32 includeVertexInput;

    VkPipeline pipeline;
};

} // namespace Tobi