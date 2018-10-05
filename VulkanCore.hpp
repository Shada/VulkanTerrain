#include <unistd.h>
#include "vulkan/vk_sdk_platform.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "WindowXcb.hpp"

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT
#define NUM_DESCRIPTOR_SETS 1

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
        VkDeviceMemory mem;
        VkImageView view;
    } ImageBuffer;

	typedef struct TUniformData
	{
		VkBuffer buffer;
        VkDeviceMemory memory;
        VkDescriptorBufferInfo bufferInfo;
    } UniformData;

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
            void initPipeline(bool includeDepth);
            

            std::unique_ptr<WindowXcb> window;

            VkSwapchainKHR swapChain;
            std::vector<SwapChainBuffer> buffers;
            uint32_t swapChainImageCount;
            uint32_t currentBuffer;

            std::vector<LayerProperties> instanceLayerProperties;
            std::vector<const char *> instanceExtensionNames;
            std::vector<const char *> deviceExtensionNames;
            std::vector<const char *> instanceLayerNames;

            VkInstance instance;
            VkDevice device;
            VkSurfaceKHR surface;

            std::vector<VkPhysicalDevice> gpus;
            uint32_t queueFamilyCount;
            std::vector<VkQueueFamilyProperties> queueProperties;
            VkPhysicalDeviceMemoryProperties memoryProperties;
            VkPhysicalDeviceProperties gpuProperties;

            uint32_t graphicsQueueFamilyIndex;
            uint32_t presentQueueFamilyIndex;

            VkFormat format;

            VkCommandPool commandPool;
            // TODO: possible to have several command buffers in the future
            VkCommandBuffer commandBuffer;

            VkQueue presentQueue;
            VkQueue graphicsQueue;

			ImageBuffer depthBuffer;
			VkFormat depthBufferFormat;

			UniformData uniformData;

            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix;
            glm::mat4 modelMatrix;
            glm::mat4 clipMatrix;
            glm::mat4 modelViewProjectionMatrix;

            std::vector<VkDescriptorSetLayout> descriptorSetLayout;
            VkPipelineLayout pipelineLayout;

            VkRenderPass renderPass;
            VkPipelineShaderStageCreateInfo shaderStages[2];

    };

}
