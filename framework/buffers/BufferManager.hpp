#pragma once

#include <map>

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
    const uint32_t createBuffer(
        const void *data,
        const uint32_t dataSize,
        VkFlags usageFlags);

  private:
    std::shared_ptr<Platform> platform;

    std::map<uint32_t, Buffer> buffers;

    uint32_t idCounter = 0;
};

} // namespace Tobi