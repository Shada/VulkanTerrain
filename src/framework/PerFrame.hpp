#pragma once

#include <vector>
#include <memory>

#include "VkCommon.hpp"
#include "FenceManager.hpp"
#include "CommandBufferManager.hpp"

namespace Tobi
{

struct PerFrame
{
    PerFrame(VkDevice device, uint32_t queueFamilyIndex);
    PerFrame(const PerFrame &) = delete;
    PerFrame(PerFrame &&) = delete;
    PerFrame &operator=(const PerFrame &) & = delete;
    PerFrame &operator=(PerFrame &&) & = delete;
    ~PerFrame();

    void beginFrame();
    VkSemaphore setSwapchainAcquireSemaphore(VkSemaphore acquireSemaphore);
    void setSwapchainReleaseSemaphore(VkSemaphore releaseSemaphore);
    void setSecondaryCommandManagersCount(uint32_t count);

    VkDevice device;
    std::shared_ptr<FenceManager> fenceManager;
    std::unique_ptr<CommandBufferManager> commandManager;
    std::vector<std::unique_ptr<CommandBufferManager>> secondaryCommandManagers;
    VkSemaphore swapchainAcquireSemaphore;
    VkSemaphore swapchainReleaseSemaphore;
    uint32_t queueIndex;
};

}
