#include "VulkanSwapChain.hpp"

namespace Tobi
{

VulkanSwapChain::VulkanSwapChain(std::shared_ptr<WindowXcb> window)
    : window(window),
      swapChain(nullptr),
      swapChainImageCount(0),
      currentBuffer(0)
{
    initSwapChain();
}

VulkanSwapChain::~VulkanSwapChain()
{
    if (swapChain)
    {
        for (uint32_t i = 0; i < swapChainImageCount; i++)
        {
            vkDestroyImageView(window->getDevice(), swapChainBuffers[i].view, nullptr);
        }
        vkDestroySwapchainKHR(window->getDevice(), swapChain, nullptr);
    }
}

void VulkanSwapChain::initSwapChain(VkImageUsageFlags usageFlags)
{ /* DEPENDS on commandBuffer and queue initialized */

    auto U_ASSERT_ONLY result = VK_SUCCESS;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(window->getPhysicalDevice(), window->getSurface(), &surfaceCapabilities);
    assert(result == VK_SUCCESS);

    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(window->getPhysicalDevice(), window->getSurface(), &presentModeCount, nullptr);
    assert(result == VK_SUCCESS);
    VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
    assert(presentModes);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(window->getPhysicalDevice(), window->getSurface(), &presentModeCount, presentModes);
    assert(result == VK_SUCCESS);

    VkExtent2D swapChainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapChainExtent.width = window->getWidth();
        swapChainExtent.height = window->getHeight();
        if (swapChainExtent.width < surfaceCapabilities.minImageExtent.width)
        {
            swapChainExtent.width = surfaceCapabilities.minImageExtent.width;
        }
        else if (swapChainExtent.width > surfaceCapabilities.maxImageExtent.width)
        {
            swapChainExtent.width = surfaceCapabilities.maxImageExtent.width;
        }

        if (swapChainExtent.height < surfaceCapabilities.minImageExtent.height)
        {
            swapChainExtent.height = surfaceCapabilities.minImageExtent.height;
        }
        else if (swapChainExtent.height > surfaceCapabilities.maxImageExtent.height)
        {
            swapChainExtent.height = surfaceCapabilities.maxImageExtent.height;
        }
    }
    else
    {
        // If the surface size is defined, the swap chain size must match
        swapChainExtent = surfaceCapabilities.currentExtent;
    }

    // The FIFO present mode is guaranteed by the spec to be supported
    // Also note that current Android driver only supports FIFO
    auto swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Determine the number of VkImage's to use in the swap chain.
    // We need to acquire only 1 presentable image at at time.
    // Asking for minImageCount images ensures that we can acquire
    // 1 presentable image as long as we present it before attempting
    // to acquire another.
    auto desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfaceCapabilities.currentTransform;
    }

    // Find a supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] =
        {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++)
    {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i])
        {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.pNext = nullptr;
    swapChainCreateInfo.surface = window->getSurface();
    swapChainCreateInfo.minImageCount = desiredNumberOfSwapChainImages;
    swapChainCreateInfo.imageFormat = window->getSurfaceFormat();
    swapChainCreateInfo.imageExtent.width = swapChainExtent.width;
    swapChainCreateInfo.imageExtent.height = swapChainExtent.height;
    swapChainCreateInfo.preTransform = preTransform;
    swapChainCreateInfo.compositeAlpha = compositeAlpha;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.presentMode = swapChainPresentMode;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
#ifndef __ANDROID__
    swapChainCreateInfo.clipped = true;
#else
    swapChainCreateInfo.clipped = false;
#endif
    swapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapChainCreateInfo.imageUsage = usageFlags;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;

    uint32_t queueFamilyIndices[2] = {window->getGraphicsQueueIndex(), window->getPresentQueueIndex()};
    if (window->getGraphicsQueueIndex() != window->getPresentQueueIndex())
    {
        // If the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between the
        // queues, or we have to create the swapchain with imageSharingMode
        // as VK_SHARING_MODE_CONCURRENT
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    result = vkCreateSwapchainKHR(window->getDevice(), &swapChainCreateInfo, nullptr, &swapChain);
    assert(result == VK_SUCCESS);

    result = vkGetSwapchainImagesKHR(window->getDevice(), swapChain, &swapChainImageCount, nullptr);
    assert(result == VK_SUCCESS);

    VkImage *swapChainImages = (VkImage *)malloc(swapChainImageCount * sizeof(VkImage));
    assert(swapChainImages);

    result = vkGetSwapchainImagesKHR(window->getDevice(), swapChain, &swapChainImageCount, swapChainImages);
    assert(result == VK_SUCCESS);

    for (uint32_t i = 0; i < swapChainImageCount; i++)
    {
        SwapChainBuffer swapChainBuffer;

        VkImageViewCreateInfo colorImageViewCreateInfo = {};
        colorImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorImageViewCreateInfo.pNext = nullptr;
        colorImageViewCreateInfo.format = window->getSurfaceFormat();
        colorImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        colorImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        colorImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        colorImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        colorImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        colorImageViewCreateInfo.subresourceRange.levelCount = 1;
        colorImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        colorImageViewCreateInfo.subresourceRange.layerCount = 1;
        colorImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorImageViewCreateInfo.flags = 0;

        swapChainBuffer.image = swapChainImages[i];

        colorImageViewCreateInfo.image = swapChainBuffer.image;

        result = vkCreateImageView(window->getDevice(), &colorImageViewCreateInfo, nullptr, &swapChainBuffer.view);
        swapChainBuffers.push_back(swapChainBuffer);
        assert(result == VK_SUCCESS);
    }
    free(swapChainImages);
    currentBuffer = 0;

    if (nullptr != presentModes)
    {
        free(presentModes);
    }
}
} // namespace Tobi