/* Copyright (c) 2016-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Context.hpp"

#include "../platform/Platform.hpp"
#include "PerFrame.hpp"
#include "buffers/VertexBufferManager.hpp"
#include "model/Model.hpp"

#include "../platform/AssetManager.hpp"

namespace Tobi
{

Context::Context()
    : platform(Platform::create()),
      backBuffers(std::vector<BackBuffer>()),
      renderPass(VK_NULL_HANDLE),
      pipelineCache(VK_NULL_HANDLE),
      pipeline(VK_NULL_HANDLE),
      pipelineLayout(VK_NULL_HANDLE),
      perFrame(std::vector<std::unique_ptr<PerFrame>>()),
      vertexBufferManager(std::make_unique<VertexBufferManager>(platform)),
      swapChainIndex(0)
{
    LOGI("CONSTRUCTING Context\n");
}

Context::~Context()
{
    LOGI("DECONSTRUCTING Context\n");

    waitIdle();

    perFrame.clear();

    terminateBackBuffers();

    if (pipelineCache)
    {
        auto device = platform->getDevice();
        vkDestroyPipelineCache(device, pipelineCache, nullptr);
        pipelineCache = VK_NULL_HANDLE;
    }
}

void Context::terminateBackBuffers()
{
    auto device = platform->getDevice();

    // Tear down backbuffers.
    // If our swapchain changes, we will call this, and create a new swapchain.
    if (!backBuffers.empty())
    {
        // Wait until device is idle before teardown.
        vkQueueWaitIdle(platform->getGraphicsQueue());
        for (auto &backBuffer : backBuffers)
        {
            vkDestroyFramebuffer(device, backBuffer.frameBuffer, nullptr);
            vkDestroyImageView(device, backBuffer.view, nullptr);
        }
        backBuffers.clear();

        vkDestroyRenderPass(device, renderPass, nullptr);
        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        renderPass = VK_NULL_HANDLE;
        pipeline = VK_NULL_HANDLE;
        pipelineLayout = VK_NULL_HANDLE;
    }
}

Result Context::presentImage(uint32_t index)
{
    return platform->presentImage(index, getSwapChainReleaseSemaphore());
}

Result Context::initialize()
{
    LOGI("START INITIALIZING Context\n");
    auto result = platform->initialize();
    if (FAILED(result))
    {
        LOGE("Failed to initialize Platform\n");
        return RESULT_ERROR_GENERIC;
    }

    // attach context to the platform ?? does it need the context in any way?

    if (FAILED(onPlatformUpdate()))
    {
        LOGE("Failed to create per frame data\n");
        return RESULT_ERROR_GENERIC;
    }

    updateSwapChain();

    LOGI("FINISHED INITIALIZING Context\n");
    return RESULT_SUCCESS;
}

const Buffer &Context::getVertexBuffer(uint32_t vertexBufferId) const
{
    return vertexBufferManager->getBuffer(vertexBufferId);
}

/// @brief Gets the fence manager for the current swapchain image.
/// Used by the platform internally.
/// @returns FenceManager
std::shared_ptr<FenceManager> &Context::getFenceManager()
{
    return perFrame[swapChainIndex]->fenceManager;
}

/// @brief Gets the acquire semaphore for the swapchain.
/// Used by the platform internally.
/// @returns Semaphore.
const VkSemaphore &Context::getSwapChainAcquireSemaphore() const
{
    return perFrame[swapChainIndex]->swapchainAcquireSemaphore;
}

/// @brief Gets the release semaphore for the swapchain.
/// Used by the platform internally.
/// @returns Semaphore.
const VkSemaphore &Context::getSwapChainReleaseSemaphore() const
{
    return perFrame[swapChainIndex]->swapchainReleaseSemaphore;
}

void Context::submit(VkCommandBuffer cmd)
{
    submitCommandBuffer(cmd, VK_NULL_HANDLE, VK_NULL_HANDLE);
}

void Context::submitSwapChain(VkCommandBuffer cmd)
{
    // For the first frames, we will create a release semaphore.
    // This can be reused every frame. Semaphores are reset when they have been
    // successfully been waited on.
    // If we aren't using acquire semaphores, we aren't using release semaphores
    // either.
    if (getSwapChainReleaseSemaphore() == VK_NULL_HANDLE && getSwapChainAcquireSemaphore() != VK_NULL_HANDLE)
    {
        VkSemaphore releaseSemaphore;
        VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VK_CHECK(vkCreateSemaphore(platform->getDevice(), &semaphoreInfo, nullptr, &releaseSemaphore));
        perFrame[swapChainIndex]->setSwapchainReleaseSemaphore(releaseSemaphore);
    }

    submitCommandBuffer(cmd, getSwapChainAcquireSemaphore(), getSwapChainReleaseSemaphore());
}

void Context::submitCommandBuffer(VkCommandBuffer cmd, VkSemaphore acquireSemaphore, VkSemaphore releaseSemaphore)
{
    // All queue submissions get a fence that CPU will wait
    // on for synchronization purposes.
    VkFence fence = getFenceManager()->requestClearedFence();

    VkSubmitInfo info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    info.commandBufferCount = 1;
    info.pCommandBuffers = &cmd;

    const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    info.waitSemaphoreCount = acquireSemaphore != VK_NULL_HANDLE ? 1 : 0;
    info.pWaitSemaphores = &acquireSemaphore;
    info.pWaitDstStageMask = &waitStage;
    info.signalSemaphoreCount = releaseSemaphore != VK_NULL_HANDLE ? 1 : 0;
    info.pSignalSemaphores = &releaseSemaphore;

    VK_CHECK(vkQueueSubmit(platform->getGraphicsQueue(), 1, &info, fence));
}

uint32_t Context::loadModel(const char *filename)
{
    LOGI("LOADING model\n");
    // TODO: load the model from file,
    // use a model manager, that holds all models(?),
    // then have an class/struct that holds all necessary model data
    // return handle to that class/struct.
    auto model = std::make_shared<Model>();

    auto vertexBufferId = vertexBufferManager->createBuffer(
        model->getVertexData(),
        model->getVertexDataSize());

    return vertexBufferId;
}

const VkCommandBuffer &Context::requestPrimaryCommandBuffer() const
{
    return perFrame[swapChainIndex]->commandManager->requestCommandBuffer();
}

const SwapChainDimensions &Context::getSwapChainDimensions() const
{
    return platform->getSwapChainDimensions();
}

Result Context::acquireNextImage(uint32_t &swapChainIndex)
{
    VkSemaphore acquireSemaphore = VK_NULL_HANDLE;
    auto result = platform->acquireNextImage(swapChainIndex, acquireSemaphore);

    while (result == RESULT_ERROR_OUTDATED_SWAPCHAIN)
    {
        result = platform->acquireNextImage(swapChainIndex, acquireSemaphore);
        updateSwapChain();
    }

    if (SUCCEEDED(result))
    {
        // Signal the underlying context that we're using this backbuffer now.
        // This will also wait for all fences associated with this swapchain image
        // to complete first.
        // When submitting command buffer that writes to swapchain, we need to wait
        // for this semaphore first.
        // Also, delete the older semaphore.
        auto oldSemaphore = beginFrame(swapChainIndex, acquireSemaphore);

        // Recycle the old semaphore back into the semaphore manager.
        if (oldSemaphore != VK_NULL_HANDLE)
        {
            platform->addClearedSemaphore(oldSemaphore);
        }

        return RESULT_SUCCESS;
    }

    return result;
}

VkSemaphore Context::beginFrame(uint32_t index, VkSemaphore acquireSemaphore)
{
    swapChainIndex = index;
    perFrame[swapChainIndex]->beginFrame();
    return perFrame[swapChainIndex]->setSwapchainAcquireSemaphore(acquireSemaphore);
}

Status Context::getWindowStatus()
{
    return platform->getWindowStatus();
}

Result Context::onPlatformUpdate()
{
    auto device = platform->getDevice();

    waitIdle();

    // Initialize per-frame resources.
    // Every swapchain image has its own command pool and fence manager.
    // This makes it very easy to keep track of when we can reset command buffers
    // and such.
    perFrame.clear();
    for (uint32_t i = 0; i < platform->getSwapChainImageCount(); i++)
    {
        perFrame.emplace_back(new PerFrame(device, platform->getGraphicsQueueFamilyIndex()));
    }

    /* setRenderingThreadCount(renderingThreadCount); */

    // Create a pipeline cache (although we'll only create one pipeline).
    VkPipelineCacheCreateInfo pipelineCacheInfo = {VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
    VK_CHECK(vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache));

    return RESULT_SUCCESS;
}

void Context::updateSwapChain()
{
    LOGI("UPDATING swap chain\n");
    auto dimensions = platform->getSwapChainDimensions();
    auto newBackBufferImages = platform->getSwapChainImages();
    auto device = platform->getDevice();

    // In case we're reinitializing the swapchain, terminate the old one first.
    terminateBackBuffers();

    // We can't initialize the renderpass until we know the swapchain format.
    initRenderPass(dimensions.format);
    // We can't initialize the pipeline until we know the render pass.
    initPipeline();

    // initialize new back buffers

    // For all backbuffers in the swapchain ...
    for (auto image : newBackBufferImages)
    {
        BackBuffer backBuffer;
        backBuffer.image = image;

        // Create an image view which we can render into.
        VkImageViewCreateInfo view = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view.format = dimensions.format;
        view.image = image;
        view.subresourceRange.baseMipLevel = 0;
        view.subresourceRange.baseArrayLayer = 0;
        view.subresourceRange.levelCount = 1;
        view.subresourceRange.layerCount = 1;
        view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view.components.r = VK_COMPONENT_SWIZZLE_R;
        view.components.g = VK_COMPONENT_SWIZZLE_G;
        view.components.b = VK_COMPONENT_SWIZZLE_B;
        view.components.a = VK_COMPONENT_SWIZZLE_A;

        VK_CHECK(vkCreateImageView(device, &view, nullptr, &backBuffer.view));

        // Build the framebuffer.
        VkFramebufferCreateInfo fbInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        fbInfo.renderPass = renderPass;
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = &backBuffer.view;
        fbInfo.width = dimensions.width;
        fbInfo.height = dimensions.height;
        fbInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &backBuffer.frameBuffer));

        backBuffers.push_back(backBuffer);
    }
}

double Context::getCurrentTime()
{
    return OS::getCurrentTime();
}

void Context::initRenderPass(VkFormat format)
{
    VkAttachmentDescription attachment = {0};
    // Backbuffer format.
    attachment.format = format;
    // Not multisampled.
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // When starting the frame, we want tiles to be cleared.
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // When ending the frame, we want tiles to be written out.
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // Don't care about stencil since we're not using it.
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // The image layout will be undefined when the render pass begins.
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // After the render pass is complete, we will transition to PRESENT_SRC_KHR
    // layout.
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // We have one subpass.
    // This subpass has 1 color attachment.
    // While executing this subpass, the attachment will be in attachment optimal
    // layout.
    VkAttachmentReference colorRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    // We will end up with two transitions.
    // The first one happens right before we start subpass #0, where
    // UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
    // The final layout in the render pass attachment states PRESENT_SRC_KHR, so
    // we
    // will get a final transition from COLOR_ATTACHMENT_OPTIMAL to
    // PRESENT_SRC_KHR.

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    // Create a dependency to external events.
    // We need to wait for the WSI semaphore to signal.
    // Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
    // actually wait for the semaphore, so we must also wait for that pipeline
    // stage.
    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // Since we changed the image layout, we need to make the memory visible to
    // color attachment to modify.
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Finally, create the renderpass.
    VkRenderPassCreateInfo rpInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    rpInfo.attachmentCount = 1;
    rpInfo.pAttachments = &attachment;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 1;
    rpInfo.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass(platform->getDevice(), &rpInfo, nullptr, &renderPass));
}

void Context::initPipeline()
{
    auto device = platform->getDevice();

    // Create a blank pipeline layout.
    // We are not binding any resources to the pipeline in this first sample.
    VkPipelineLayoutCreateInfo layoutInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &pipelineLayout));

    // Specify we will use triangle lists to draw geometry.
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Specify our two attributes, Position and Color.
    VkVertexInputAttributeDescription attributes[2] = {{0}};
    attributes[0].location = 0; // Position in shader specifies layout(location =
    // 0) to link with this attribute.
    attributes[0].binding = 0; // Uses vertex buffer #0.
    attributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[0].offset = 0;
    attributes[1].location = 1; // Color in shader specifies layout(location = 1)
    // to link with this attribute.
    attributes[1].binding = 0; // Uses vertex buffer #0.
    attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[1].offset = 4 * sizeof(float);

    // We have one vertex buffer, with stride 8 floats (vec4 + vec4).
    VkVertexInputBindingDescription binding = {0};
    binding.binding = 0;
    binding.stride = sizeof(Vertex); // We specify the buffer stride up front here.
    // The vertex buffer will step for every vertex (rather than per instance).
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInput = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &binding;
    vertexInput.vertexAttributeDescriptionCount = 2;
    vertexInput.pVertexAttributeDescriptions = attributes;

    // Specify rasterization state.
    VkPipelineRasterizationStateCreateInfo raster = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.cullMode = VK_CULL_MODE_BACK_BIT;
    raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    raster.depthClampEnable = false;
    raster.rasterizerDiscardEnable = false;
    raster.depthBiasEnable = false;
    raster.lineWidth = 1.0f;

    // Our attachment will write to all color channels, but no blending is
    // enabled.
    VkPipelineColorBlendAttachmentState blendAttachment = {0};
    blendAttachment.blendEnable = false;
    blendAttachment.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo blend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    blend.attachmentCount = 1;
    blend.pAttachments = &blendAttachment;

    // We will have one viewport and scissor box.
    VkPipelineViewportStateCreateInfo viewport = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;

    // Disable all depth testing.
    VkPipelineDepthStencilStateCreateInfo depthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depthStencil.depthTestEnable = false;
    depthStencil.depthWriteEnable = false;
    depthStencil.depthBoundsTestEnable = false;
    depthStencil.stencilTestEnable = false;

    // No multisampling.
    VkPipelineMultisampleStateCreateInfo multisample = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Specify that these states will be dynamic, i.e. not part of pipeline state
    // object.
    static const VkDynamicState dynamics[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamic = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic.pDynamicStates = dynamics;
    dynamic.dynamicStateCount = sizeof(dynamics) / sizeof(dynamics[0]);

    // Load our SPIR-V shaders.
    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
        {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
    };

    // We have two pipeline stages, vertex and fragment.
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = loadShaderModule(device, "shaders/triangle.vert.spv");
    shaderStages[0].pName = "main";
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = loadShaderModule(device, "shaders/triangle.frag.spv");
    shaderStages[1].pName = "main";

    VkGraphicsPipelineCreateInfo pipe = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipe.stageCount = 2;
    pipe.pStages = shaderStages;
    pipe.pVertexInputState = &vertexInput;
    pipe.pInputAssemblyState = &inputAssembly;
    pipe.pRasterizationState = &raster;
    pipe.pColorBlendState = &blend;
    pipe.pMultisampleState = &multisample;
    pipe.pViewportState = &viewport;
    pipe.pDepthStencilState = &depthStencil;
    pipe.pDynamicState = &dynamic;

    // We need to specify the pipeline layout and the render pass description up
    // front as well.
    pipe.renderPass = renderPass;
    pipe.layout = pipelineLayout;

    VK_CHECK(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipe, nullptr, &pipeline));

    // Pipeline is baked, we can delete the shader modules now.
    vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device, shaderStages[1].module, nullptr);
}

void Context::waitIdle()
{
    platform->waitIdle();
}

VkShaderModule Context::loadShaderModule(VkDevice device, const char *pPath)
{
    std::vector<uint32_t> buffer;
    if (FAILED(OS::getAssetManager().readBinaryFile(&buffer, pPath)))
    {
        LOGE("Failed to read SPIR-V file: %s.\n", pPath);
        return VK_NULL_HANDLE;
    }

    VkShaderModuleCreateInfo moduleInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    moduleInfo.codeSize = buffer.size() * sizeof(uint32_t);
    moduleInfo.pCode = buffer.data();

    VkShaderModule shaderModule;
    VK_CHECK(vkCreateShaderModule(device, &moduleInfo, nullptr, &shaderModule));
    return shaderModule;
}

} // namespace Tobi
