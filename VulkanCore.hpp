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

#include "EventDispatcher.hpp"

// Amount of time, in nanoseconds, to wait for a command buffer to complete
#define FENCE_TIMEOUT 100000000

namespace Tobi
{
typedef struct TTextureData
{
    VkDescriptorImageInfo imageInfo;
} TextureData;

class VulkanCore
    : public Dispatcher<ResizeWindowEvent>::Listener
{
  public:
    VulkanCore(std::shared_ptr<ResizeWindowDispatcher> resizeWindowDispatcher);
    ~VulkanCore() = default;

    void recreateSwapChain()
    {
        frameBuffers->clean();
        commandBuffer->clean();
        pipeline->clean();
        depthBuffer->clean();
        renderPass->clean();
        swapChain->clean();

        swapChain->create();
        renderPass->create();
        pipeline->create();
        depthBuffer->create();
        frameBuffers->create();
        commandBuffer->create();
    }
    virtual void onEvent(ResizeWindowEvent &event, Dispatcher<ResizeWindowEvent> &sender)
    {
        // recreate swapchain here.
        std::cout << "SwapChain: onEvent ResizeWindowEvent " << event.width << "x" << event.height << std::endl;

        recreateSwapChain();
    }

    void run();

  private:
    // would be nice to be able to configure functionality from a configuration
    // GUI or a configuration file
    // but for now, all the common core stuff should be initialized here.
    void initVulkan();

    VkResult initGlobalLayerProperties();
    VkResult initGlobalExtensionProperties(LayerProperties &layerProperties);

    void initDescriptorSet(bool useTexture);
    void initScissors();
    void initViewPorts();

    void drawFrame();

    std::vector<LayerProperties> instanceLayerProperties;

    std::shared_ptr<ResizeWindowDispatcher> resizeWindowDispatcher;

    TextureData textureData;

    std::shared_ptr<WindowXcb> window;

    std::unique_ptr<Camera> camera;

    std::shared_ptr<VulkanCommandPool> commandPool;

    std::unique_ptr<VulkanCommandBuffer> commandBuffer;

    std::shared_ptr<VulkanSwapChain> swapChain;

    std::shared_ptr<VulkanDepthBuffer> depthBuffer;

    std::unique_ptr<VulkanUniformBuffer> uniformBuffer;

    std::shared_ptr<VulkanRenderPass> renderPass;

    std::shared_ptr<VulkanShaderProgram> shaderProgram;

    std::shared_ptr<VulkanVertexBuffer> vertexBuffer;

    std::shared_ptr<VulkanFrameBuffers> frameBuffers;

    std::shared_ptr<VulkanPipelineCache> pipelineCache;
    std::unique_ptr<VulkanPipeline> pipeline;

    std::unique_ptr<VulkanDescriptorPool> descriptorPool;

    std::vector<VkDescriptorSet> descriptorSets;

    VkRect2D scissor;
    VkViewport viewPort;
};

} // namespace Tobi
