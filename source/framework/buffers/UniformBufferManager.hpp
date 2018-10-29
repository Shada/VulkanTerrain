#pragma once
#include "BufferManager.hpp"

namespace Tobi
{

class UniformBufferManager : public BufferManager
{
  public:
    UniformBufferManager(std::shared_ptr<Platform> platform);
    ~UniformBufferManager() = default;

    const uint32_t createBuffer(
        const void *data,
        const uint32_t dataSize);
};

} // namespace Tobi