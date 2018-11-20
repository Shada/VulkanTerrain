#include "IndexBufferManager.hpp"

namespace Tobi
{

IndexBufferManager::IndexBufferManager(
    std::shared_ptr<Platform> platform)
    : BufferManager(platform)
{
    LOGI("CONSTRUCTING IndexBufferManager\n");
}

const uint32_t IndexBufferManager::createBuffer(
    const void *data,
    const uint32_t dataSize)
{
    return BufferManager::createBuffer(
        data,
        dataSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

} // namespace Tobi