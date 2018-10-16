#pragma once

#include <vulkan/vulkan.hpp>

#include "../WindowXcb.hpp"
#include "VulkanPipelineCache.hpp"
#include "../VulkanRenderPass.hpp"
#include "../Shader/VulkanShaderProgram.hpp"
#include "../Buffers/VulkanVertexBuffer.hpp"

namespace Tobi
{

// Number of descriptor sets needs to be the same at alloc,
// pipeline layout creation, and descriptor set layout creation
#define NUM_DESCRIPTOR_SETS 1

class VulkanPipeline
{
  public:
    VulkanPipeline(
        std::shared_ptr<WindowXcb> window,
        std::shared_ptr<VulkanPipelineCache> pipelineCache,
        std::shared_ptr<VulkanRenderPass> renderPass,
        std::shared_ptr<VulkanShaderProgram> shaderProgram,
        std::shared_ptr<VulkanVertexBuffer> vertexBuffer,
        VkBool32 useTexture,
        VkBool32 includeDepth,
        VkBool32 includeVertexInput = VK_TRUE);
    VulkanPipeline(const VulkanPipeline &) = delete;
    VulkanPipeline(VulkanPipeline &&) = default;
    VulkanPipeline &operator=(const VulkanPipeline &) & = delete;
    VulkanPipeline &operator=(VulkanPipeline &&) & = default;
    ~VulkanPipeline();

    void create();
    void clean();

    const VkPipeline &getPipeline() { return pipeline; }
    const VkPipelineLayout &getPipelineLayout() { return pipelineLayout; }
    const std::vector<VkDescriptorSetLayout> &getDescriptorSetLayouts() { return descriptorSetLayouts; }

  private:
    void initDescriptorLayouts(
        VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags = 0);
    void initPipelineLayout();
    void initPipeline();

    std::shared_ptr<WindowXcb> window;
    std::shared_ptr<VulkanPipelineCache> pipelineCache;
    std::shared_ptr<VulkanRenderPass> renderPass;
    std::shared_ptr<VulkanShaderProgram> shaderProgram;
    std::shared_ptr<VulkanVertexBuffer> vertexBuffer;
    VkBool32 useTexture;
    VkBool32 includeDepth;
    VkBool32 includeVertexInput;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
};

} // namespace Tobi