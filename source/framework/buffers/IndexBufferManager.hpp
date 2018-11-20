#pragma once

#include "BufferManager.hpp"

namespace Tobi
{

class IndexBufferManager : public BufferManager
{
  public:
    IndexBufferManager(std::shared_ptr<Platform> platform);
    IndexBufferManager(const IndexBufferManager &) = delete;
    IndexBufferManager(IndexBufferManager &&) = delete;
    IndexBufferManager &operator=(const IndexBufferManager &) & = delete;
    IndexBufferManager &operator=(IndexBufferManager &&) & = delete;
    ~IndexBufferManager() = default;

    const uint32_t createBuffer(
        const void *data,
        const uint32_t dataSize);
};

} // namespace Tobi