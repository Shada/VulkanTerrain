#pragma once
#include "BufferManager.hpp"

namespace Tobi
{

class VertexBufferManager : public BufferManager
{
  public:
    VertexBufferManager(std::shared_ptr<Platform> platform);
    ~VertexBufferManager() = default;

    const Buffer &createBuffer(
        const void *data,
        const uint32_t dataSize);
};

} // namespace Tobi