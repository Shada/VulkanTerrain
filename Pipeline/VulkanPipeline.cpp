#include "VulkanPipeline.hpp"

#include <cassert>
#include <cstring>

#include "AssertTypeDefine.hpp"
#include "util.hpp"

namespace Tobi
{

VulkanPipeline::VulkanPipeline(
    std::shared_ptr<WindowXcb> window,
    std::shared_ptr<VulkanPipelineCache> pipelineCache,
    std::shared_ptr<VulkanRenderPass> renderPass,
    std::shared_ptr<VulkanShaderProgram> shaderProgram,
    std::shared_ptr<VulkanVertexBuffer> vertexBuffer,
    VkBool32 useTexture,
    VkBool32 includeDepth,
    VkBool32 includeVertexInput)
    : window(window),
      pipelineCache(pipelineCache),
      renderPass(renderPass),
      shaderProgram(shaderProgram),
      vertexBuffer(vertexBuffer),
      useTexture(useTexture),
      includeDepth(includeDepth),
      includeVertexInput(includeVertexInput),
      pipeline(nullptr),
      pipelineLayout(nullptr),
      descriptorSetLayouts(std::vector<VkDescriptorSetLayout>())
{
    initDescriptorLayouts();
    initPipelineLayout();
    initPipeline();
}

VulkanPipeline::~VulkanPipeline()
{
    if (pipelineLayout)
    {
        for (auto &descriptorSetLayout : descriptorSetLayouts)
            vkDestroyDescriptorSetLayout(window->getDevice(), descriptorSetLayout, nullptr);
        vkDestroyPipelineLayout(window->getDevice(), pipelineLayout, nullptr);
    }
    vkDestroyPipeline(window->getDevice(), pipeline, nullptr);
}

void VulkanPipeline::initDescriptorLayouts(
    VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags)
{
    auto U_ASSERT_ONLY result = VK_SUCCESS;
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

    descriptorSetLayouts.resize(NUM_DESCRIPTOR_SETS);
    result = vkCreateDescriptorSetLayout(window->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayouts.data());
    assert(result == VK_SUCCESS);
}

void VulkanPipeline::initPipelineLayout()
{
    auto U_ASSERT_ONLY result = VK_SUCCESS;
    // Now use the descriptor layout to create a pipelineCreateInfo layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();

    result = vkCreatePipelineLayout(window->getDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    assert(result == VK_SUCCESS);
}
void VulkanPipeline::initPipeline()
{
    auto U_ASSERT_ONLY result = VK_SUCCESS;

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

    result = vkCreateGraphicsPipelines(window->getDevice(), pipelineCache->getPipelineCache(), 1, &pipelineCreateInfo, nullptr, &pipeline);
    assert(result == VK_SUCCESS);
}

void VulkanPipeline::create()
{
    initPipelineLayout();
    initPipeline();
}

void VulkanPipeline::clean()
{
    if (pipelineLayout)
    {
        vkDestroyPipelineLayout(window->getDevice(), pipelineLayout, nullptr);
    }
    vkDestroyPipeline(window->getDevice(), pipeline, nullptr);
}

} // namespace Tobi