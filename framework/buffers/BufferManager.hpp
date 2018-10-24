#pragma once

#include "../Common.hpp"
#include "../../platform/Platform.hpp"

namespace Tobi
{

struct Buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDescriptorBufferInfo bufferInfo;
};

class BufferManager
{
  public:
    BufferManager(std::shared_ptr<Platform> platform);
    virtual ~BufferManager();

    /// Creates a buffer and returns the id
    const Buffer &createBuffer(
        const void *data,
        const uint32_t dataSize,
        VkFlags usageFlags);

  private:
    std::shared_ptr<Platform> platform;

    std::vector<Buffer> buffers;
};

} // namespace Tobi