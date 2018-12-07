#pragma once

#include "BufferManager.hpp"

namespace Tobi
{

class VertexBufferManager : public BufferManager
{
  public:
    VertexBufferManager(std::shared_ptr<Platform> platform);
    VertexBufferManager(const VertexBufferManager &) = delete;
    VertexBufferManager(VertexBufferManager &&) = delete;
    VertexBufferManager &operator=(const VertexBufferManager &) & = delete;
    VertexBufferManager &operator=(VertexBufferManager &&) & = delete;
    ~VertexBufferManager() = default;

    const uint32_t createBuffer(
        const void *data,
        const uint32_t dataSize);
};

} // namespace Tobi