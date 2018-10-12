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
      vertexBuffer(nullptr)
{
    WindowSettings settings;
    settings.width = 800;
    settings.height = 800;

    window = std::make_shared<WindowXcb>(settings);

    initVulkan();
}
VulkanCore::~VulkanCore()
{
    vkDestroyPipeline(window->getDevice(), pipeline, nullptr);

    vkDestroyPipelineCache(window->getDevice(), pipelineCache, nullptr);

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

    shaderProgram = std::make_unique<VulkanShaderProgram>(window);

    frameBuffers = std::make_unique<VulkanFrameBuffers>(
        window,
        depthBuffer,
        renderPass,
        swapChain,
        depthPresent);

    vertexBuffer = std::make_unique<VulkanVertexBuffer>(window, cubeData, sizeof(cubeData), sizeof(cubeData[0]), false);

    descriptorPool = std::make_unique<VulkanDescriptorPool>(window, false);

    initDescriptorSet(false);
    initPipelineCache();
    initPipeline(depthPresent);

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

    vkCmdBindPipeline(commandBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
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
void VulkanCore::initPipelineCache()
{
    VkResult U_ASSERT_ONLY result;

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo;
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheCreateInfo.pNext = nullptr;
    pipelineCacheCreateInfo.initialDataSize = 0;
    pipelineCacheCreateInfo.pInitialData = nullptr;
    pipelineCacheCreateInfo.flags = 0;
    result = vkCreatePipelineCache(window->getDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache);
    assert(result == VK_SUCCESS);
}
void VulkanCore::initPipeline(VkBool32 includeDepth, VkBool32 includeVertexInput)
{
    VkResult U_ASSERT_ONLY result;

    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    memset(&vertexInputStateCreateInfo, 0, sizeof(vertexInputStateCreateInfo));
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (includeVertexInput)
    {
        vertexInputStateCreateInfo.pNext = nullptr;
        vertexInputStateCreateInfo.flags = 0;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBuffer->getVertexInputBinding();
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexBuffer->getVertexInputAttributeDescription();
    }
    VkPipelineInputAssemblyStateCreateInfo inpitAssemblyStateCreateInfo;
    inpitAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inpitAssemblyStateCreateInfo.pNext = nullptr;
    inpitAssemblyStateCreateInfo.flags = 0;
    inpitAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    inpitAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0;
    rasterizationStateCreateInfo.depthBiasClamp = 0;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.flags = 0;
    colorBlendStateCreateInfo.pNext = nullptr;
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState[1];
    colorBlendAttachmentState[0].colorWriteMask = 0xf;
    colorBlendAttachmentState[0].blendEnable = VK_FALSE;
    colorBlendAttachmentState[0].alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState[0].colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = colorBlendAttachmentState;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendStateCreateInfo.blendConstants[0] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 1.0f;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = nullptr;
    viewportStateCreateInfo.flags = 0;
#ifndef __ANDROID__
    viewportStateCreateInfo.viewportCount = NUM_VIEWPORTS;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    viewportStateCreateInfo.scissorCount = NUM_SCISSORS;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
    viewportStateCreateInfo.pScissors = nullptr;
    viewportStateCreateInfo.pViewports = nullptr;
#else
    // Temporary disabling dynamic viewport on Android because some of drivers doesn't
    // support the feature.
    VkViewport viewports;
    viewports.minDepth = 0.0f;
    viewports.maxDepth = 1.0f;
    viewports.x = 0;
    viewports.y = 0;
    viewports.width = window->getWidth();
    viewports.height = window->getHeight();
    VkRect2D scissor;
    scissor.extent.width = window->getWidth();
    scissor.extent.height = window->getHeight();
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    viewportStateCreateInfo.viewportCount = NUM_VIEWPORTS;
    viewportStateCreateInfo.scissorCount = NUM_SCISSORS;
    viewportStateCreateInfo.pScissors = &scissor;
    viewportStateCreateInfo.pViewports = &viewports;
#endif
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.pNext = nullptr;
    depthStencilStateCreateInfo.flags = 0;
    depthStencilStateCreateInfo.depthTestEnable = includeDepth;
    depthStencilStateCreateInfo.depthWriteEnable = includeDepth;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilStateCreateInfo.back.compareMask = 0;
    depthStencilStateCreateInfo.back.reference = 0;
    depthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.back.writeMask = 0;
    depthStencilStateCreateInfo.minDepthBounds = 0;
    depthStencilStateCreateInfo.maxDepthBounds = 0;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = depthStencilStateCreateInfo.back;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.pNext = nullptr;
    multisampleStateCreateInfo.flags = 0;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.rasterizationSamples = NUM_SAMPLES;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    multisampleStateCreateInfo.minSampleShading = 0.0;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inpitAssemblyStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.pStages = shaderProgram->getShaderStages().data();
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.renderPass = renderPass->getRenderPass();
    pipelineCreateInfo.subpass = 0;

    result = vkCreateGraphicsPipelines(window->getDevice(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline);
    assert(result == VK_SUCCESS);
}
} // namespace Tobi
