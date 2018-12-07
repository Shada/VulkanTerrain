#pragma once

#include <map>

#include "Buffer.hpp"

#include "../VkCommon.hpp"
#include "../../platform/Platform.hpp"

namespace Tobi
{

class BufferManager
{
  public:
    BufferManager(std::shared_ptr<Platform> platform);
    BufferManager(const BufferManager &) = delete;
    BufferManager(BufferManager &&) = delete;
    BufferManager &operator=(const BufferManager &) & = delete;
    BufferManager &operator=(BufferManager &&) & = delete;
    virtual ~BufferManager();

    /// Creates a buffer and returns the id
    const uint32_t createBuffer(
        const void *data,
        const uint32_t dataSize,
        VkFlags usageFlags);

    const Buffer &getBuffer(uint32_t index)
    {
        return buffers[index];
    }

    const VkDescriptorBufferInfo &getBufferInfo(uint32_t index)
    {
        return buffers[index].bufferInfo;
    }

  private:
    std::shared_ptr<Platform> platform;

    std::map<uint32_t, Buffer> buffers;

    uint32_t idCounter = 0;
};

} // namespace Tobi
