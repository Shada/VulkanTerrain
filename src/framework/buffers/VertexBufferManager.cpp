#include "VertexBufferManager.hpp"

namespace Tobi
{

VertexBufferManager::VertexBufferManager(
    std::shared_ptr<Platform> platform)
    : BufferManager(platform)
{
    LOGI("CONSTRUCTING VertexBufferManager\n");
}

const uint32_t VertexBufferManager::createBuffer(
    const void *data,
    const uint32_t dataSize)
{
    return BufferManager::createBuffer(
        data,
        dataSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

} // namespace Tobi