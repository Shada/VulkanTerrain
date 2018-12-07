#pragma once

#include "BufferManager.hpp"

namespace Tobi
{

class UniformBufferManager : public BufferManager
{
  public:
    UniformBufferManager(std::shared_ptr<Platform> platform);
    UniformBufferManager(const UniformBufferManager &) = delete;
    UniformBufferManager(UniformBufferManager &&) = delete;
    UniformBufferManager &operator=(const UniformBufferManager &) & = delete;
    UniformBufferManager &operator=(UniformBufferManager &&) & = delete;
    ~UniformBufferManager() = default;

    const uint32_t createBuffer(
        const void *data,
        const uint32_t dataSize);
};

} // namespace Tobi