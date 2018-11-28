#include "VulkanDevice.hpp"

namespace TobiEngine
{

VkDeviceWrapper::VkDeviceWrapper()
{
    VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
}
} // namespace TobiEngine