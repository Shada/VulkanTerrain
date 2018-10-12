#include "VulkanPipelineCache.hpp"

#include "AssertTypeDefine.hpp"

namespace Tobi
{

VulkanPipelineCache::VulkanPipelineCache(std::shared_ptr<WindowXcb> window)
    : window(window),
      pipelineCache(nullptr)
{
    initPipelineCache();
}

VulkanPipelineCache::~VulkanPipelineCache()
{
    if (pipelineCache)
    {
        vkDestroyPipelineCache(window->getDevice(), pipelineCache, nullptr);
    }
}

void VulkanPipelineCache::initPipelineCache()
{
    VkResult U_ASSERT_ONLY result;

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheCreateInfo.pNext = nullptr;
    pipelineCacheCreateInfo.initialDataSize = 0;
    pipelineCacheCreateInfo.pInitialData = nullptr;
    pipelineCacheCreateInfo.flags = 0;

    result = vkCreatePipelineCache(window->getDevice(), &pipelineCacheCreateInfo, nullptr, &pipelineCache);
    assert(result == VK_SUCCESS);
}

} // namespace Tobi