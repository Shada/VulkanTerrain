#pragma once

#include <vulkan/vulkan.hpp>

#include "WindowXcb.hpp"

namespace Tobi
{

    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(std::shared_ptr<WindowXcb> window);
        VulkanCommandPool(const VulkanCommandPool&) = delete;
        VulkanCommandPool(VulkanCommandPool&&) = default;
        VulkanCommandPool& operator=(const VulkanCommandPool&) & = delete;
        VulkanCommandPool& operator=(VulkanCommandPool&&) & = default;
        ~VulkanCommandPool();

        const VkCommandPool &getCommandPool() { return commandPool; }

    private:
        void initCommandPool();

        std::shared_ptr<WindowXcb> window;

        VkCommandPool commandPool;
    };

}  // namespace Tobi