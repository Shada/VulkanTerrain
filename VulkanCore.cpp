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
WindowSettings settings{800, 800, "TobiApp"};

const bool depthPresent = true;

VulkanCore::VulkanCore() // TODO: do the make_shared here (in correct order).
    : resizeWindowDispatcher(std::make_shared<ResizeWindowDispatcher>()),
      window(std::make_shared<WindowXcb>(settings, resizeWindowDispatcher)),
      camera(std::make_unique<Camera>(settings)),
      commandPool(std::make_shared<VulkanCommandPool>(window)),
      commandBuffer(std::make_unique<VulkanCommandBuffer>(window, commandPool)),
      swapChain(std::make_shared<VulkanSwapChain>(window)),
      depthBuffer(std::make_shared<VulkanDepthBuffer>(window)),
      uniformBuffer(std::make_unique<VulkanUniformBuffer>(
          window,
          (void *)&camera->getModelViewProjectionMatrix(),
          sizeof(camera->getModelViewProjectionMatrix()))),
      renderPass(std::make_shared<VulkanRenderPass>(window, depthBuffer, depthPresent)),
      shaderProgram(std::make_shared<VulkanShaderProgram>(window)),
      frameBuffers(std::make_unique<VulkanFrameBuffers>(
          window,
          depthBuffer,
          renderPass,
          swapChain,
          depthPresent)),
      vertexBuffer(std::make_shared<VulkanVertexBuffer>(
          window,
          cubeData,
          sizeof(cubeData),
          sizeof(cubeData[0]),
          false)),
      pipelineCache(std::make_shared<VulkanPipelineCache>(window)),
      pipeline(std::make_unique<VulkanPipeline>(
          window,
          pipelineCache,
          renderPass,
          shaderProgram,
          vertexBuffer,
          VK_FALSE,
          depthPresent)),
      descriptorPool(std::make_unique<VulkanDescriptorPool>(window, false)),
      descriptorSets(std::vector<VkDescriptorSet>())
{
    resizeWindowDispatcher->Reg(swapChain);

    initVulkan();

    resizeWindowDispatcher->Unreg(swapChain);
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
    // TODO: this should be moved to where the properties are applied (not currently in use)
    auto result = initGlobalLayerProperties();
    if (result != VK_SUCCESS)
    {
        std::cout << "Failed to init GlobalLayerProperties. "
                  << "Res: " << result << std::endl;
        return;
    }

    initDescriptorSet(false);

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

    swapChain->aquireNextImage(imageAcquiredSemaphore);

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
    vkCmdBindDescriptorSets(commandBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipelineLayout(), 0, NUM_DESCRIPTOR_SETS,
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

    while (window->isRunning())
    {
        window->pollEvents();
    }
}

void drawFrame()
{
    // 1. aquire next swapchain
    // and handle VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR (basically recreate swapchain)

    // 2. construct command buffers
    // some command buffers might be prerecorded, for things that rarely changes

    // 3. submit command buffers to queue
    // and wait for semaphores / fences

    // 4. present result
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

void VulkanCore::initDescriptorSet(bool useTexture)
{
    // DEPENDS on init_descriptor_pool()

    VkResult U_ASSERT_ONLY result;

    VkDescriptorSetAllocateInfo descriptorsetAllocationInfo[1];
    descriptorsetAllocationInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorsetAllocationInfo[0].pNext = nullptr;
    descriptorsetAllocationInfo[0].descriptorPool = descriptorPool->getDescriptorPool();
    descriptorsetAllocationInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    descriptorsetAllocationInfo[0].pSetLayouts = pipeline->getDescriptorSetLayouts().data();

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
