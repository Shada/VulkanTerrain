#include "VulkanDepthBuffer.hpp"
#include "util.hpp"

namespace Tobi
{
VulkanDepthBuffer::VulkanDepthBuffer(std::shared_ptr<WindowXcb> window)
    : format(VK_FORMAT_UNDEFINED),
      imageView(nullptr),
      image(nullptr),
      memory(nullptr),
      window(window)
{
    initDepthBuffer();
}

VulkanDepthBuffer::~VulkanDepthBuffer()
{
    if (imageView)
        vkDestroyImageView(window->getDevice(), imageView, nullptr);
    if (image)
        vkDestroyImage(window->getDevice(), image, nullptr);
    if (memory)
        vkFreeMemory(window->getDevice(), memory, nullptr);
}

void VulkanDepthBuffer::initDepthBuffer()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;
    bool U_ASSERT_ONLY pass = true;
    VkImageCreateInfo imageCreateInfo = {};

    // allow custom depth formats
#ifdef __ANDROID__
    // Depth format needs to be VK_FORMAT_D24_UNORM_S8_UINT on Android.
    format = VK_FORMAT_D24_UNORM_S8_UINT;
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    if (format == VK_FORMAT_UNDEFINED)
        format = VK_FORMAT_D32_SFLOAT;
#else
    if (format == VK_FORMAT_UNDEFINED)
        format = VK_FORMAT_D16_UNORM;
#endif

    const VkFormat depthFormat = format;
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(window->getPhysicalDevice(), depthFormat, &formatProperties);
    if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    }
    else if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    }
    else
    {
        // Try other depth formats?
        std::cout << "depth_format " << depthFormat << " Unsupported.\n";
        exit(-1);
    }

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = depthFormat;
    imageCreateInfo.extent.width = window->getWidth();
    imageCreateInfo.extent.height = window->getHeight();
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = NUM_SAMPLES;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.flags = 0;

    VkMemoryAllocateInfo memoryAllocationInfo = {};
    memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocationInfo.pNext = nullptr;
    memoryAllocationInfo.allocationSize = 0;
    memoryAllocationInfo.memoryTypeIndex = 0;

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = nullptr;
    imageViewCreateInfo.image = VK_NULL_HANDLE;
    imageViewCreateInfo.format = depthFormat;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.flags = 0;

    if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT || depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkMemoryRequirements memoryRequirements;

    // Create image
    result = vkCreateImage(window->getDevice(), &imageCreateInfo, nullptr, &image);
    assert(result == VK_SUCCESS);

    vkGetImageMemoryRequirements(window->getDevice(), image, &memoryRequirements);

    memoryAllocationInfo.allocationSize = memoryRequirements.size;
    // Use the memory properties to determine the type of memory required
    pass =
        window->memoryTypeFromProperties(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memoryAllocationInfo.memoryTypeIndex);
    assert(pass);

    // Allocate memory
    result = vkAllocateMemory(window->getDevice(), &memoryAllocationInfo, nullptr, &memory);
    assert(result == VK_SUCCESS);

    // Bind memory
    result = vkBindImageMemory(window->getDevice(), image, memory, 0);
    assert(result == VK_SUCCESS);

    // Create image view
    imageViewCreateInfo.image = image;
    result = vkCreateImageView(window->getDevice(), &imageViewCreateInfo, nullptr, &imageView);
    assert(result == VK_SUCCESS);
}

} // namespace Tobi