#include "PerFrame.hpp"

namespace Tobi
{

PerFrame::PerFrame(VkDevice device, uint32_t queueFamilyIndex)
    : device(device),
      fenceManager(std::make_shared<FenceManager>(device)),
      commandManager(std::make_unique<CommandBufferManager>(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, queueFamilyIndex)),
      secondaryCommandManagers(std::vector<std::unique_ptr<CommandBufferManager>>()),
      swapchainAcquireSemaphore(VK_NULL_HANDLE),
      swapchainReleaseSemaphore(VK_NULL_HANDLE),
      queueIndex(queueFamilyIndex)
{
}

PerFrame::~PerFrame()
{
    if (swapchainAcquireSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(device, swapchainAcquireSemaphore, nullptr);
    if (swapchainReleaseSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(device, swapchainReleaseSemaphore, nullptr);
}

void PerFrame::beginFrame()
{
    fenceManager->beginFrame();
    commandManager->beginFrame();
    for (auto &pManager : secondaryCommandManagers)
        pManager->beginFrame();
}

void PerFrame::setSecondaryCommandManagersCount(uint32_t count)
{
    secondaryCommandManagers.clear();
    for (uint32_t i = 0; i < count; i++)
    {
        secondaryCommandManagers.emplace_back(
            new CommandBufferManager(device, VK_COMMAND_BUFFER_LEVEL_SECONDARY, queueIndex));
    }
}

VkSemaphore PerFrame::setSwapchainAcquireSemaphore(VkSemaphore acquireSemaphore)
{
    VkSemaphore ret = swapchainAcquireSemaphore;
    swapchainAcquireSemaphore = acquireSemaphore;
    return ret;
}

void PerFrame::setSwapchainReleaseSemaphore(VkSemaphore releaseSemaphore)
{
    if (swapchainReleaseSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(device, swapchainReleaseSemaphore, nullptr);
    swapchainReleaseSemaphore = releaseSemaphore;
}

}