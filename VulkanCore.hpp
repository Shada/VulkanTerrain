#include <unistd.h>
#include "vulkan/vk_sdk_platform.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "WindowXcb.hpp"

// Number of samples needs to be the same at image creation,      
// renderpass creation and pipeline creation.                     
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

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
    typedef struct 
    {
        VkLayerProperties properties;
        std::vector<VkExtensionProperties> instanceExtensions;
        std::vector<VkExtensionProperties> deviceExtensions;
    } LayerProperties;

    typedef struct TSwapChainBuffer 
    {
        VkImage image;
        VkImageView view;
    } SwapChainBuffer;

    typedef struct TImageBuffer
    {
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
    } ImageBuffer;

	typedef struct TUniformData
	{
		VkBuffer buffer;
        VkDeviceMemory memory;
        VkDescriptorBufferInfo bufferInfo;
    } UniformData;

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

            void initInstanceExtensionNames();
            
            void initDeviceExtensionNames();
           
            VkResult initDeviceExtensionProperties(LayerProperties &layerProperties);

            VkResult initDevice();
            VkResult initEnumerateDevice(uint32_t gpu_count = 1);
            VkResult initInstance(char const *const appShortName);

            void initSwapchainExtension();
           
            void initCommandPool();
            void initCommandBuffer();

            void executeBeginCommandBuffer();
            void executeEndCommandBuffer(); 

            void initDeviceQueue();

            void initSwapChain(VkImageUsageFlags usageFlags = 
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

			void initDepthBuffer();                
            void initUniformBuffer();
            
			bool memoryTypeFromProperties(uint32_t typeBits,
                                          	 VkFlags requirements_mask,
                                 			 uint32_t *typeIndex);
			
            void initDescriptorAndPipelineLayouts(
                    bool useTexture,
                    VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags = 0);
            void initRenderpass(
                    bool includeDepth, 
                    bool clear = true, 
                    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            void initShaders(const char *vertexShaderText, const char *fragmentShaderText);
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

            std::unique_ptr<WindowXcb> window;

            VkSwapchainKHR swapChain;
            std::vector<SwapChainBuffer> swapChainBuffers;
            uint32_t swapChainImageCount;
            uint32_t currentBuffer;

            std::vector<LayerProperties> instanceLayerProperties;
            std::vector<const char *> instanceExtensionNames;
            std::vector<const char *> deviceExtensionNames;
            std::vector<const char *> instanceLayerNames;

            VkInstance instance;
            VkDevice device;
            VkSurfaceKHR surface;

            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;

            VkPipelineCache pipelineCache;
            VkPipeline pipeline;

            std::vector<VkPhysicalDevice> gpus;
            uint32_t queueFamilyCount;
            std::vector<VkQueueFamilyProperties> queueProperties;
            VkPhysicalDeviceMemoryProperties memoryProperties;
            VkPhysicalDeviceProperties gpuProperties;

            uint32_t graphicsQueueFamilyIndex;
            uint32_t presentQueueFamilyIndex;

            VkFormat format;

            VkCommandPool commandPool;
            // TODO: possible to have several command swapChainBuffers in the future
            VkCommandBuffer commandBuffer;

            VkQueue presentQueue;
            VkQueue graphicsQueue;

			ImageBuffer depthBuffer;
			VkFormat depthBufferFormat;

            VkFramebuffer *frameBuffers;

			UniformData uniformData;

            TextureData textureData;

            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix;
            glm::mat4 modelMatrix;
            glm::mat4 clipMatrix;
            glm::mat4 modelViewProjectionMatrix;

            std::vector<VkDescriptorSetLayout> descriptorSetLayout;
            VkPipelineLayout pipelineLayout;

            VkRenderPass renderPass;
            VkPipelineShaderStageCreateInfo shaderStages[2];

            VkVertexInputBindingDescription vertexInputBinding;
            VkVertexInputAttributeDescription vertexInputAttributes[2];
            VertexBuffer vertexBuffer;
    };

}
