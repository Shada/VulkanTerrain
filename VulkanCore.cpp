#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <cstring>

#include "VulkanCore.hpp"

#include "Utils/util.hpp"
#include "Utils/AssertTypeDefine.hpp"

#include "CubeVertexData.hpp"

namespace Tobi
{
VulkanCore::VulkanCore()
    : commandPool(nullptr),
      commandBuffer(nullptr),
      swapChain(nullptr),
      depthBuffer(nullptr),
      window(nullptr),
      shaderProgram(nullptr),
      uniformBuffer(nullptr),
      pipelineLayout(nullptr),
      renderPass(nullptr),
      vertexBuffer(nullptr),
      pipelineCache(nullptr),
      pipeline(nullptr)
{
    WindowSettings settings;
    settings.width = 800;
    settings.height = 800;

    window = std::make_shared<WindowXcb>(settings);

    initVulkan();
}
VulkanCore::~VulkanCore()
{
    if (pipelineLayout)
    {
        for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)
            vkDestroyDescriptorSetLayout(window->getDevice(), descriptorSetLayout[i], nullptr);
        vkDestroyPipelineLayout(window->getDevice(), pipelineLayout, nullptr);
    }
}

void waitSeconds(int seconds)
{
#ifdef WIN32
    Sleep(seconds * 1000);
#elif defined(__ANDROID__)
    sleep(seconds);
#else
    sleep(seconds);
#endif
}

void VulkanCore::initVulkan()
{
    auto result = initGlobalLayerProperties();
    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to init GlobalLayerProperties. "
                  << "Res: " << result << std::endl;
        return;
    }

    const bool depthPresent = true;

    commandPool = std::make_shared<VulkanCommandPool>(window);

    commandBuffer = std::make_unique<VulkanCommandBuffer>(window, commandPool);

    swapChain = std::make_shared<VulkanSwapChain>(window);

    depthBuffer = std::make_shared<VulkanDepthBuffer>(window);

    camera = std::make_unique<Camera>(window);

    uniformBuffer = std::make_unique<VulkanUniformBuffer>(
        window,
        (void *)&camera->getModelViewProjectionMatrix(),
        sizeof(camera->getModelViewProjectionMatrix()));

    initDescriptorAndPipelineLayouts(false);

    renderPass = std::make_shared<VulkanRenderPass>(window, depthBuffer, depthPresent);

    shaderProgram = std::make_shared<VulkanShaderProgram>(window);

    frameBuffers = std::make_unique<VulkanFrameBuffers>(
        window,
        depthBuffer,
        renderPass,
        swapChain,
        depthPresent);

    vertexBuffer = std::make_shared<VulkanVertexBuffer>(window, cubeData, sizeof(cubeData), sizeof(cubeData[0]), false);

    descriptorPool = std::make_unique<VulkanDescriptorPool>(window, false);

    initDescriptorSet(false);

    pipelineCache = std::make_shared<VulkanPipelineCache>(window);

    pipeline = std::make_unique<VulkanPipeline>(
        window,
        pipelineCache,
        renderPass,
        shaderProgram,
        vertexBuffer,
        pipelineLayout,
        depthPresent);

    // VULKAN_KEY_START

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.2f;
    clearValues[0].color.float32[1] = 0.2f;
    clearValues[0].color.float32[2] = 0.2f;
    clearValues[0].color.float32[3] = 0.2f;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    VkSemaphore imageAcquiredSemaphore;
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    result = vkCreateSemaphore(window->getDevice(), &imageAcquiredSemaphoreCreateInfo, nullptr, &imageAcquiredSemaphore);
    assert(result == VK_SUCCESS);

    // Get the index of the next available swapchain image:
    // TODO: move to swapchain class
    result = vkAcquireNextImageKHR(window->getDevice(), swapChain->getSwapChain(), UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE,
                                   &swapChain->getCurrentBuffer());
    // TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
    // return codes
    assert(result == VK_SUCCESS);

    VkRenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = renderPass->getRenderPass();
    renderPassBeginInfo.framebuffer = frameBuffers->getCurrentFrameBuffer();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = window->getWidth();
    renderPassBeginInfo.renderArea.extent.height = window->getHeight();
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    commandBuffer->executeBeginCommandBuffer();

    vkCmdBeginRenderPass(commandBuffer->getCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());
    vkCmdBindDescriptorSets(commandBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, NUM_DESCRIPTOR_SETS,
                            descriptorSets.data(), 0, nullptr);

    const VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer->getCommandBuffer(), 0, 1, &vertexBuffer->getBuffer(), offsets);

    initViewPorts();
    initScissors();

    vkCmdDraw(commandBuffer->getCommandBuffer(), 12 * 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer->getCommandBuffer());

    commandBuffer->executeEndCommandBuffer();

    const VkCommandBuffer commandBuffers[] = {commandBuffer->getCommandBuffer()};
    VkFenceCreateInfo fenceCreateInfo;
    VkFence drawFence;
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;
    vkCreateFence(window->getDevice(), &fenceCreateInfo, nullptr, &drawFence);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo[1] = {};
    submitInfo[0].pNext = nullptr;
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].waitSemaphoreCount = 1;
    submitInfo[0].pWaitSemaphores = &imageAcquiredSemaphore;
    submitInfo[0].pWaitDstStageMask = &pipelineStageFlags;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = commandBuffers;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    /* Queue the command buffer for execution */
    result = vkQueueSubmit(window->getGraphicsQueue(), 1, submitInfo, drawFence);
    assert(result == VK_SUCCESS);

    /* Now present the image in the window */

    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain->getSwapChain();
    present.pImageIndices = &swapChain->getCurrentBuffer();
    present.pWaitSemaphores = nullptr;
    present.waitSemaphoreCount = 0;
    present.pResults = nullptr;

    /* Make sure command buffer is finished before presenting */
    do
    {
        result = vkWaitForFences(window->getDevice(), 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);

    assert(result == VK_SUCCESS);
    result = vkQueuePresentKHR(window->getPresentQueue(), &present);
    assert(result == VK_SUCCESS);

    waitSeconds(1);
    /* VULKAN_KEY_END */

    vkDestroySemaphore(window->getDevice(), imageAcquiredSemaphore, nullptr);
    vkDestroyFence(window->getDevice(), drawFence, nullptr);
}

void VulkanCore::initViewPorts()
{
#ifdef __ANDROID__
    // Disable dynamic viewport on Android. Some drive has an issue with the dynamic viewport
    // feature.
#else
    viewPort.height = (float)window->getWidth();
    viewPort.width = (float)window->getHeight();
    viewPort.minDepth = (float)0.0f;
    viewPort.maxDepth = (float)1.0f;
    viewPort.x = 0;
    viewPort.y = 0;
    vkCmdSetViewport(commandBuffer->getCommandBuffer(), 0, NUM_VIEWPORTS, &viewPort);
#endif
}
void VulkanCore::initScissors()
{
#ifdef __ANDROID__
    // Disable dynamic viewport on Android. Some drive has an issue with the dynamic scissors
    // feature.
#else
    scissor.extent.width = window->getWidth();
    scissor.extent.height = window->getHeight();
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(commandBuffer->getCommandBuffer(), 0, NUM_SCISSORS, &scissor);
#endif
}

VkResult VulkanCore::initGlobalLayerProperties()
{
    VkResult result = VK_SUCCESS;
    uint32_t instanceLayerCount;
    VkLayerProperties *vkLayerProperties = nullptr;
    do
    {
        result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        if (result)
        {
            return result;
        }

        if (instanceLayerCount == 0)
        {
            return VK_SUCCESS;
        }

        vkLayerProperties = (VkLayerProperties *)realloc(
            vkLayerProperties,
            instanceLayerCount * sizeof(VkLayerProperties));

        result = vkEnumerateInstanceLayerProperties(
            &instanceLayerCount,
            vkLayerProperties);
    } while (result == VK_INCOMPLETE);

    for (uint32_t i = 0; i < instanceLayerCount; i++)
    {
        LayerProperties layerProperties;
        layerProperties.properties = vkLayerProperties[i];
        result = initGlobalExtensionProperties(layerProperties);
        if (result)
            return result;
        instanceLayerProperties.push_back(layerProperties);
    }
    free(vkLayerProperties);

    return result;
}

VkResult VulkanCore::initGlobalExtensionProperties(LayerProperties &layerProperties)
{
    VkExtensionProperties *instanceExtensions;
    uint32_t instanceExtensionCount;
    VkResult result = VK_SUCCESS;

    auto layerName = layerProperties.properties.layerName;

    do
    {
        result = vkEnumerateInstanceExtensionProperties(
            layerName,
            &instanceExtensionCount,
            nullptr);

        if (result)
        {
            return result;
        }

        if (instanceExtensionCount == 0)
        {
            return VK_SUCCESS;
        }

        layerProperties.instanceExtensions.resize(instanceExtensionCount);
        instanceExtensions = layerProperties.instanceExtensions.data();
        result = vkEnumerateInstanceExtensionProperties(
            layerName,
            &instanceExtensionCount,
            instanceExtensions);
    } while (result == VK_INCOMPLETE);

    return result;
}

void VulkanCore::initDescriptorAndPipelineLayouts(
    bool useTexture,
    VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags)
{
    VkDescriptorSetLayoutBinding layoutBindings[2];
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[0].pImmutableSamplers = nullptr;

    if (useTexture)
    {
        layoutBindings[1].binding = 1;
        layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBindings[1].descriptorCount = 1;
        layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBindings[1].pImmutableSamplers = nullptr;
    }

    // Next take layout bindings and use them to create a descriptor set layout
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.flags = descriptorSetLayoutCreateFlags;
    descriptorSetLayoutCreateInfo.bindingCount = useTexture ? 2 : 1;
    descriptorSetLayoutCreateInfo.pBindings = layoutBindings;

    VkResult U_ASSERT_ONLY result = VK_SUCCESS;

    descriptorSetLayout.resize(NUM_DESCRIPTOR_SETS);
    result = vkCreateDescriptorSetLayout(window->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayout.data());
    assert(result == VK_SUCCESS);

    // Now use the descriptor layout to create a pipelineCreateInfo layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.data();

    result = vkCreatePipelineLayout(window->getDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    assert(result == VK_SUCCESS);
}
void VulkanCore::initDescriptorSet(bool useTexture)
{
    // DEPENDS on init_descriptor_pool()

    VkResult U_ASSERT_ONLY result;

    VkDescriptorSetAllocateInfo descriptorsetAllocationInfo[1];
    descriptorsetAllocationInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorsetAllocationInfo[0].pNext = nullptr;
    descriptorsetAllocationInfo[0].descriptorPool = descriptorPool->getDescriptorPool();
    descriptorsetAllocationInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    descriptorsetAllocationInfo[0].pSetLayouts = descriptorSetLayout.data();

    descriptorSets.resize(NUM_DESCRIPTOR_SETS);
    result = vkAllocateDescriptorSets(window->getDevice(), descriptorsetAllocationInfo, descriptorSets.data());
    assert(result == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];

    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = descriptorSets[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &uniformBuffer->getBufferInfo();
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    if (useTexture)
    {
        writes[1] = {};
        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstSet = descriptorSets[0];
        writes[1].dstBinding = 1;
        writes[1].descriptorCount = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = &textureData.imageInfo;
        writes[1].dstArrayElement = 0;
    }

    vkUpdateDescriptorSets(window->getDevice(), useTexture ? 2 : 1, writes, 0, nullptr);
}

} // namespace Tobi
