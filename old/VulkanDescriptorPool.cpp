#include "VulkanDescriptorPool.hpp"

#include <cassert>

#include "Utils/AssertTypeDefine.hpp"

namespace Tobi
{

VulkanDescriptorPool::VulkanDescriptorPool(std::shared_ptr<WindowXcb> window, bool useTexture)
    : window(window),
      useTexture(useTexture),
      descriptorPool(nullptr)
{
    initDescriptorPool();
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    if (descriptorPool)
        vkDestroyDescriptorPool(window->getDevice(), descriptorPool, nullptr);
}

void VulkanDescriptorPool::initDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].descriptorCount = 1;
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if (useTexture)
    {
        poolSizes[1].descriptorCount = 1;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
    descriptorPoolCreateInfo.poolSizeCount = useTexture ? 2 : 1;

    VkResult U_ASSERT_ONLY result = vkCreateDescriptorPool(window->getDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);
    assert(result == VK_SUCCESS);
}

} // namespace Tobi