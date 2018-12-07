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
#include "buffers/IndexBufferManager.hpp"
#include "buffers/UniformBufferManager.hpp"
#include "model/Model.hpp"

#include "../platform/AssetManager.hpp"

#include "EventDispatchers.hpp"

namespace Tobi
{

Context::Context()
    : platform(Platform::create()),
      depthBufferFormat(VK_FORMAT_D16_UNORM),
      backBuffers(std::vector<BackBuffer>()),
      renderPass(VK_NULL_HANDLE),
      pipelineCache(VK_NULL_HANDLE),
      pipeline(VK_NULL_HANDLE),
      pipelineLayout(VK_NULL_HANDLE),
      perFrame(std::vector<std::unique_ptr<PerFrame>>()),
      vertexBufferManager(std::make_shared<VertexBufferManager>(platform)),
      indexBufferManager(std::make_shared<IndexBufferManager>(platform)),
      uniformBufferManager(std::make_shared<UniformBufferManager>(platform)),
      swapChainIndex(0),
      camera(nullptr),
      keyStates(std::make_shared<KeyStates>()),
      modelManager(std::make_unique<ModelManager>(vertexBufferManager,
                                                  indexBufferManager)),
      objectManager(std::make_unique<ObjectManager>())
{
    LOGI("CONSTRUCTING Context\n");
    EventDispatchersStruct::keyPressDispatcher->Reg(keyStates);
    EventDispatchersStruct::keyReleaseDispatcher->Reg(keyStates);
}

Context::~Context()
{
    LOGI("DECONSTRUCTING Context\n");

    EventDispatchersStruct::keyPressDispatcher->Unreg(keyStates);
    EventDispatchersStruct::keyReleaseDispatcher->Unreg(keyStates);

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

        vkDestroyImageView(device, depthBufferView, nullptr);
        vkDestroyImage(device, depthBufferImage, nullptr);
        vkFreeMemory(device, depthBufferMemory, nullptr);
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

    camera = std::make_shared<Camera>(platform->getSwapChainDimensions());

    auto triangleModelId = loadModel("triangle");
    auto cubeModelId = loadModel("cube");
    auto spiderModelId = loadModel("assets/models/spider.fbx");
    auto cube2ModelId = loadModel("assets/models/BindPose.fbx");

    triangleId = objectManager->addObject(triangleModelId, {0, 1, 0}, {0, 0, 0}, {1.5, 1.5, 1.5});
    cubeId = objectManager->addObject(cubeModelId, {1, 0, 0}, {0, 0, 0}, {0.5, 0.5, 0.5});
    spiderId = objectManager->addObject(spiderModelId, {1.0, 1.0, -5}, {0, M_PI, 0}, {0.0001f, 0.0001f, 0.0001f});
    cube2Id = objectManager->addObject(cube2ModelId, {0.0, -0.5, 2.5}, {0, M_PI, 0}, {0.1f, 0.1f, 0.1f});

    LOGI("FINISHED INITIALIZING Context\n");
    return RESULT_SUCCESS;
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

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.waitSemaphoreCount = acquireSemaphore != VK_NULL_HANDLE ? 1 : 0;
    submitInfo.pWaitSemaphores = &acquireSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.signalSemaphoreCount = releaseSemaphore != VK_NULL_HANDLE ? 1 : 0;
    submitInfo.pSignalSemaphores = &releaseSemaphore;

    VK_CHECK(vkQueueSubmit(platform->getGraphicsQueue(), 1, &submitInfo, fence));
}

uint32_t Context::loadModel(const char *filename)
{
    LOGI("LOADING model\n");
    // TODO: load the model from file,
    // use a model manager, that holds all models(?),
    // then have an class/struct that holds all necessary model data
    // return handle to that class/struct.

    auto modelId = modelManager->loadModel(filename);

    return modelId;
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

TobiStatus Context::getWindowStatus()
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

    initDepthBuffer(dimensions.width, dimensions.height);

    // We can't initialize the renderpass until we know the swapchain format.
    initRenderPass(dimensions.format);
    // We can't initialize the pipeline until we know the render pass.

    initPipeline();

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
        VkImageView attachments[2] = {backBuffer.view, depthBufferView};
        VkFramebufferCreateInfo fbInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        fbInfo.renderPass = renderPass;
        fbInfo.attachmentCount = 2;
        fbInfo.pAttachments = attachments;
        fbInfo.width = dimensions.width;
        fbInfo.height = dimensions.height;
        fbInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &backBuffer.frameBuffer));

        backBuffers.push_back(backBuffer);
    }
}

void Context::initDepthBuffer(uint32_t width, uint32_t height)
{
    VkDevice device = platform->getDevice();

    if (!platform->getSupportedDepthFormat(&depthBufferFormat))
    {
        LOGE("Could not find supported depth format!");
        throw std::runtime_error("Could not find supported depth format!");
    }

    // Create the image for the depth buffer. Note that imageInfo.usage includes the
    // VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT flag. This flag allows lazy allocation of the depth buffer.
    // For Mali GPU this flag will allow the driver to never allocate memory for the depth buffer and use the
    // on-chip tile buffer instead.
    VkImageCreateInfo imageInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = depthBufferFormat;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &depthBufferImage));

    // Allocate memory for the depth image. Prefer a memory type with lazily allocation support.
    VkMemoryRequirements memoryRequirements = {0};
    vkGetImageMemoryRequirements(device, depthBufferImage, &memoryRequirements);
    uint32_t memoryTypeIndex = platform->findMemoryTypeFromRequirementsWithFallback(memoryRequirements.memoryTypeBits,
                                                                                    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);

    VkMemoryAllocateInfo memInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memInfo.allocationSize = memoryRequirements.size;
    memInfo.memoryTypeIndex = memoryTypeIndex;

    VK_CHECK(vkAllocateMemory(device, &memInfo, nullptr, &depthBufferMemory));
    VK_CHECK(vkBindImageMemory(device, depthBufferImage, depthBufferMemory, 0));

    // Create the depth buffer image.
    VkImageViewCreateInfo viewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    viewInfo.image = depthBufferImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthBufferFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &depthBufferView));
}

double Context::getCurrentTime()
{
    return OS::getCurrentTime();
}

void Context::initRenderPass(VkFormat format)
{
    std::array<VkAttachmentDescription, 2> attachments = {};
    // Setup the color attachment.
    attachments[0].format = format;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    // Setup the depth attachment.
    attachments[1].format = depthBufferFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentReference depthAttachmentRef = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpassDescription = {0};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentRef;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;
    subpassDescription.pResolveAttachments = nullptr;

    // Create a dependency to external events.
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Finally, create the renderpass.
    VkRenderPassCreateInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    VK_CHECK(vkCreateRenderPass(platform->getDevice(), &renderPassInfo, nullptr, &renderPass));
}

void Context::initPipeline()
{
    auto device = platform->getDevice();

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ShaderDataBlock);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

    // Specify we will use triangle lists to draw geometry.
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Specify our two attributes, Position, Normal, and Color.
    VkVertexInputAttributeDescription attributes[3] = {{0}};
    attributes[0].location = 0; // Position in shader specifies layout(location =
    // 0) to link with this attribute.
    attributes[0].binding = 0; // Uses vertex buffer #0.
    attributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[0].offset = 0;
    attributes[1].location = 1; // Normal in shader specifies layout(location = 1)
    // to link with this attribute.
    attributes[1].binding = 0; // Uses vertex buffer #0.
    attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[1].offset = 3 * sizeof(float);
    attributes[2].location = 2; // Color in shader specifies layout(location = 1)
    // to link with this attribute.
    attributes[2].binding = 0; // Uses vertex buffer #0.
    attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[2].offset = 6 * sizeof(float);

    // We have one vertex buffer, with stride 12 floats (vec4 + vec4 + vec4).
    VkVertexInputBindingDescription binding = {0};
    binding.binding = 0;
    binding.stride = sizeof(Vertex); // We specify the buffer stride up front here.
    // The vertex buffer will step for every vertex (rather than per instance).
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInput = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &binding;
    vertexInput.vertexAttributeDescriptionCount = 3;
    vertexInput.pVertexAttributeDescriptions = attributes;

    // Specify rasterization state.
    VkPipelineRasterizationStateCreateInfo raster = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.cullMode = VK_CULL_MODE_NONE;
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
    depthStencil.depthTestEnable = true;
    depthStencil.depthWriteEnable = true;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
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

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = shaderStages;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInput;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    graphicsPipelineCreateInfo.pRasterizationState = &raster;
    graphicsPipelineCreateInfo.pColorBlendState = &blend;
    graphicsPipelineCreateInfo.pMultisampleState = &multisample;
    graphicsPipelineCreateInfo.pViewportState = &viewport;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencil;
    graphicsPipelineCreateInfo.pDynamicState = &dynamic;

    // We need to specify the pipeline layout and the render pass description up
    // front as well.
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.layout = pipelineLayout;

    VK_CHECK(vkCreateGraphicsPipelines(device, pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline));

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

Result Context::update(float time)
{
    // TODO:change to within epsilon
    if (time == 0.0)
        return RESULT_SUCCESS;

    camera->update(time);

    shaderDataBlock.viewProjectionMatrix = camera->getViewProjectionMatrix();

    return RESULT_SUCCESS;
}

Result Context::render()
{
    // Request a fresh command buffer.
    auto cmd = requestPrimaryCommandBuffer();

    // We will only submit this once before it's recycled.
    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    // Set clear color values.
    VkClearValue clearValues[2] = {0};
    clearValues[0].color.float32[0] = 0.1f;
    clearValues[0].color.float32[1] = 0.1f;
    clearValues[0].color.float32[2] = 0.2f;
    clearValues[0].color.float32[3] = 1.0f;
    clearValues[1].depthStencil.depth = 1.0f;

    // Begin the render pass.
    auto dim = getSwapChainDimensions();
    VkRenderPassBeginInfo rpBegin = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpBegin.renderPass = renderPass;
    rpBegin.framebuffer = getBackBuffer(swapChainIndex).frameBuffer;
    rpBegin.renderArea.extent.width = dim.width;
    rpBegin.renderArea.extent.height = dim.height;
    rpBegin.clearValueCount = 2;
    rpBegin.pClearValues = clearValues;

    // We will add draw commands in the same command buffer.
    vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline.
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // Set up dynamic state.
    // Viewport
    VkViewport vp = {0};
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.width = float(dim.width);
    vp.height = float(dim.height);
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &vp);

    // Scissor box
    VkRect2D scissor;
    memset(&scissor, 0, sizeof(scissor));
    scissor.extent.width = dim.width;
    scissor.extent.height = dim.height;
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // draw triangle

    // Bind vertex buffer.
    VkDeviceSize offset = 0;
    auto triangleModelId = objectManager->getMeshIndex(triangleId);
    auto vbId = modelManager->getVertexBufferIndex(triangleModelId);

    shaderDataBlock.modelMatrix = objectManager->getModelMatrix(triangleId);

    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShaderDataBlock), &shaderDataBlock);

    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBufferManager->getBuffer(vbId).buffer, &offset);
    vkCmdBindIndexBuffer(cmd, indexBufferManager->getBuffer(vbId).buffer, offset, VK_INDEX_TYPE_UINT32);

    // Draw three vertices with one instance.
    vkCmdDrawIndexed(cmd, modelManager->getModel(triangleModelId)->getIndexCount(), 1, 0, 0, 0);

    // draw cube

    // Bind vertex buffer.
    auto cubeModelId = objectManager->getMeshIndex(cubeId);
    vbId = modelManager->getVertexBufferIndex(cubeModelId);
    shaderDataBlock.modelMatrix = objectManager->getModelMatrix(cubeId);

    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShaderDataBlock), &shaderDataBlock);

    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBufferManager->getBuffer(vbId).buffer, &offset);
    vkCmdBindIndexBuffer(cmd, indexBufferManager->getBuffer(vbId).buffer, offset, VK_INDEX_TYPE_UINT32);

    // Draw three vertices with one instance.
    vkCmdDrawIndexed(cmd, modelManager->getModel(cubeModelId)->getIndexCount(), 1, 0, 0, 0);

    // draw spider

    // Bind vertex buffer.
    auto spiderModelId = objectManager->getMeshIndex(spiderId);
    vbId = modelManager->getVertexBufferIndex(spiderModelId);
    shaderDataBlock.modelMatrix = objectManager->getModelMatrix(spiderId);

    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShaderDataBlock), &shaderDataBlock);

    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBufferManager->getBuffer(vbId).buffer, &offset);
    vkCmdBindIndexBuffer(cmd, indexBufferManager->getBuffer(vbId).buffer, offset, VK_INDEX_TYPE_UINT32);

    auto howmany = modelManager->getModel(spiderModelId)->getIndexCount();
    // Draw three vertices with one instance.
    vkCmdDrawIndexed(cmd, howmany, 1, 0, 0, 0);

    // draw cube 2

    // Bind vertex buffer.
    auto cube2ModelId = objectManager->getMeshIndex(cube2Id);
    vbId = modelManager->getVertexBufferIndex(cube2ModelId);
    shaderDataBlock.modelMatrix = objectManager->getModelMatrix(cube2Id);

    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShaderDataBlock), &shaderDataBlock);

    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBufferManager->getBuffer(vbId).buffer, &offset);
    vkCmdBindIndexBuffer(cmd, indexBufferManager->getBuffer(vbId).buffer, offset, VK_INDEX_TYPE_UINT32);

    howmany = modelManager->getModel(cube2ModelId)->getIndexCount();
    // Draw three vertices with one instance.
    vkCmdDrawIndexed(cmd, howmany, 1, 0, 0, 0);

    // Complete render pass.
    vkCmdEndRenderPass(cmd);

    // Complete the command buffer.
    VK_CHECK(vkEndCommandBuffer(cmd));

    // Submit it to the queue.
    submitSwapChain(cmd);

    //present
    return presentImage(swapChainIndex);
}

} // namespace Tobi
