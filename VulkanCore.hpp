#pragma once

#include <unistd.h>
#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "WindowXcb.hpp"
#include "Shader/VulkanShaderProgram.hpp"
#include "VulkanSwapChain.hpp"
#include "Buffers/VulkanDepthBuffer.hpp"
#include "Buffers/VulkanUniformBuffer.hpp"
#include "Command/VulkanCommandPool.hpp"
#include "Command/VulkanCommandBuffer.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanRenderPass.hpp"
#include "Buffers/VulkanFrameBuffers.hpp"
#include "Buffers/VulkanVertexBuffer.hpp"
#include "Camera.hpp"
#include "Pipeline/VulkanPipelineCache.hpp"
#include "Pipeline/VulkanPipeline.hpp"

// Number of descriptor sets needs to be the same at alloc,
// pipeline layout creation, and descriptor set layout creation
#define NUM_DESCRIPTOR_SETS 1

// Amount of time, in nanoseconds, to wait for a command buffer to complete
#define FENCE_TIMEOUT 100000000

namespace Tobi
{
typedef struct TTextureData
{
    VkDescriptorImageInfo imageInfo;
} TextureData;

class VulkanCore
{
  public:
    VulkanCore();
    ~VulkanCore();

  private:
    // would be nice to be able to configure functionality from a configuration
    // GUI or a configuration file
    // but for now, all the common core stuff should be initialized here.
    void initVulkan();

    VkResult initGlobalLayerProperties();
    VkResult initGlobalExtensionProperties(LayerProperties &layerProperties);

    void initDescriptorAndPipelineLayouts(
        bool useTexture,
        VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags = 0);
    void initDescriptorSet(bool useTexture);
    void initScissors();
    void initViewPorts();

    VkRect2D scissor;
    VkViewport viewPort;

    std::shared_ptr<WindowXcb> window;

    std::shared_ptr<VulkanShaderProgram> shaderProgram;

    std::shared_ptr<VulkanSwapChain> swapChain;

    std::shared_ptr<VulkanDepthBuffer> depthBuffer;

    std::unique_ptr<VulkanUniformBuffer> uniformBuffer;

    std::shared_ptr<VulkanCommandPool> commandPool;

    std::unique_ptr<VulkanCommandBuffer> commandBuffer;

    std::unique_ptr<VulkanDescriptorPool> descriptorPool;

    std::shared_ptr<VulkanRenderPass> renderPass;

    std::unique_ptr<VulkanFrameBuffers> frameBuffers;

    std::shared_ptr<VulkanVertexBuffer> vertexBuffer;

    std::unique_ptr<Camera> camera;

    std::shared_ptr<VulkanPipelineCache> pipelineCache;
    std::unique_ptr<VulkanPipeline> pipeline;

    std::vector<LayerProperties> instanceLayerProperties;

    std::vector<VkDescriptorSet> descriptorSets;

    TextureData textureData;

    std::vector<VkDescriptorSetLayout> descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
};

} // namespace Tobi
