#pragma once
#include "vulkan/vulkan.hpp"

namespace TobiEngine
{

class VkDeviceWrapper
{
  public:
    VkDeviceWrapper();

  private:
    VkInstance instance;
};

} // namespace TobiEngine