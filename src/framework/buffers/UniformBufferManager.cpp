#include "UniformBufferManager.hpp"

namespace Tobi
{

UniformBufferManager::UniformBufferManager(
    std::shared_ptr<Platform> platform)
    : BufferManager(platform)
{
    LOGI("CONSTRUCTING UniformBufferManager\n");
}

const uint32_t UniformBufferManager::createBuffer(
    const void *data,
    const uint32_t dataSize)
{
    return BufferManager::createBuffer(
        data,
        dataSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

} // namespace Tobi