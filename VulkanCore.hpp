#pragma once

#include <unistd.h>
#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "WindowXcb.hpp"
#include "VulkanShaderProgram.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanDepthBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "VulkanCommandPool.hpp"


// Number of descriptor sets needs to be the same at alloc,       
// pipeline layout creation, and descriptor set layout creation   
#define NUM_DESCRIPTOR_SETS 1


// Number of viewports and number of scissors have to be the same 
// at pipeline creation and in any call to set them dynamically   
// They also have to be the same as each other                    
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS

// Amount of time, in nanoseconds, to wait for a command buffer to complete 
#define FENCE_TIMEOUT 100000000

namespace Tobi
{
    typedef struct TVertexBuffer
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkDescriptorBufferInfo bufferInfo;
    } VertexBuffer;
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

           
            void initCommandPool();
            void initCommandBuffer();

            void executeBeginCommandBuffer();
            void executeEndCommandBuffer(); 

            void initCameraMatrices();
			
            void initDescriptorAndPipelineLayouts(
                    bool useTexture,
                    VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags = 0);
            void initRenderpass(
                    bool includeDepth, 
                    bool clear = true, 
                    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            void initFrameBuffers(bool includeDepth);
            void initVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);
            void initDescriptorPool(bool useTexture);
            void initDescriptorSet(bool useTexture);
            void initPipelineCache();
            void initPipeline(VkBool32 includeDepth, VkBool32 includeVertexInput = VK_TRUE);
            void initScissors();
            void initViewPorts();

            VkRect2D scissor;
            VkViewport viewPort;

            std::shared_ptr<WindowXcb> window;

            std::unique_ptr<VulkanShaderProgram> shaderProgram;

            std::unique_ptr<VulkanSwapChain> swapChain;

            std::unique_ptr<VulkanDepthBuffer> depthBuffer;

            std::unique_ptr<VulkanUniformBuffer> uniformBuffer;

            std::unique_ptr<VulkanCommandPool> commandPool;

            std::vector<LayerProperties> instanceLayerProperties;


            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;

            VkPipelineCache pipelineCache;
            VkPipeline pipeline;

            // TODO: possible to have several command buffers in the future
            VkCommandBuffer commandBuffer;

            VkFramebuffer *frameBuffers;

            TextureData textureData;

            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix;
            glm::mat4 modelMatrix;
            glm::mat4 clipMatrix;
            glm::mat4 modelViewProjectionMatrix;

            std::vector<VkDescriptorSetLayout> descriptorSetLayout;
            VkPipelineLayout pipelineLayout;

            VkRenderPass renderPass;

            VkVertexInputBindingDescription vertexInputBinding;
            VkVertexInputAttributeDescription vertexInputAttributes[2];
            VertexBuffer vertexBuffer;
    };

}  // namespace Tobi
