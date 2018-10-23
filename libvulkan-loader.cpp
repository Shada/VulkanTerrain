#include "libvulkan-loader.hpp"

PFN_vkCreateInstance vulkanSymbolWrapper_vkCreateInstance;
PFN_vkEnumerateInstanceExtensionProperties vulkanSymbolWrapper_vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vulkanSymbolWrapper_vkEnumerateInstanceLayerProperties;
PFN_vkDestroyInstance vulkanSymbolWrapper_vkDestroyInstance;
PFN_vkEnumeratePhysicalDevices vulkanSymbolWrapper_vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceFeatures vulkanSymbolWrapper_vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceFormatProperties vulkanSymbolWrapper_vkGetPhysicalDeviceFormatProperties;
PFN_vkGetPhysicalDeviceImageFormatProperties vulkanSymbolWrapper_vkGetPhysicalDeviceImageFormatProperties;
PFN_vkGetPhysicalDeviceProperties vulkanSymbolWrapper_vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vulkanSymbolWrapper_vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceMemoryProperties vulkanSymbolWrapper_vkGetPhysicalDeviceMemoryProperties;
PFN_vkGetDeviceProcAddr vulkanSymbolWrapper_vkGetDeviceProcAddr;
PFN_vkCreateDevice vulkanSymbolWrapper_vkCreateDevice;
PFN_vkDestroyDevice vulkanSymbolWrapper_vkDestroyDevice;
PFN_vkEnumerateDeviceExtensionProperties vulkanSymbolWrapper_vkEnumerateDeviceExtensionProperties;
PFN_vkEnumerateDeviceLayerProperties vulkanSymbolWrapper_vkEnumerateDeviceLayerProperties;
PFN_vkGetDeviceQueue vulkanSymbolWrapper_vkGetDeviceQueue;
PFN_vkQueueSubmit vulkanSymbolWrapper_vkQueueSubmit;
PFN_vkQueueWaitIdle vulkanSymbolWrapper_vkQueueWaitIdle;
PFN_vkDeviceWaitIdle vulkanSymbolWrapper_vkDeviceWaitIdle;
PFN_vkAllocateMemory vulkanSymbolWrapper_vkAllocateMemory;
PFN_vkFreeMemory vulkanSymbolWrapper_vkFreeMemory;
PFN_vkMapMemory vulkanSymbolWrapper_vkMapMemory;
PFN_vkUnmapMemory vulkanSymbolWrapper_vkUnmapMemory;
PFN_vkFlushMappedMemoryRanges vulkanSymbolWrapper_vkFlushMappedMemoryRanges;
PFN_vkInvalidateMappedMemoryRanges vulkanSymbolWrapper_vkInvalidateMappedMemoryRanges;
PFN_vkGetDeviceMemoryCommitment vulkanSymbolWrapper_vkGetDeviceMemoryCommitment;
PFN_vkBindBufferMemory vulkanSymbolWrapper_vkBindBufferMemory;
PFN_vkBindImageMemory vulkanSymbolWrapper_vkBindImageMemory;
PFN_vkGetBufferMemoryRequirements vulkanSymbolWrapper_vkGetBufferMemoryRequirements;
PFN_vkGetImageMemoryRequirements vulkanSymbolWrapper_vkGetImageMemoryRequirements;
PFN_vkGetImageSparseMemoryRequirements vulkanSymbolWrapper_vkGetImageSparseMemoryRequirements;
PFN_vkGetPhysicalDeviceSparseImageFormatProperties vulkanSymbolWrapper_vkGetPhysicalDeviceSparseImageFormatProperties;
PFN_vkQueueBindSparse vulkanSymbolWrapper_vkQueueBindSparse;
PFN_vkCreateFence vulkanSymbolWrapper_vkCreateFence;
PFN_vkDestroyFence vulkanSymbolWrapper_vkDestroyFence;
PFN_vkResetFences vulkanSymbolWrapper_vkResetFences;
PFN_vkGetFenceStatus vulkanSymbolWrapper_vkGetFenceStatus;
PFN_vkWaitForFences vulkanSymbolWrapper_vkWaitForFences;
PFN_vkCreateSemaphore vulkanSymbolWrapper_vkCreateSemaphore;
PFN_vkDestroySemaphore vulkanSymbolWrapper_vkDestroySemaphore;
PFN_vkCreateEvent vulkanSymbolWrapper_vkCreateEvent;
PFN_vkDestroyEvent vulkanSymbolWrapper_vkDestroyEvent;
PFN_vkGetEventStatus vulkanSymbolWrapper_vkGetEventStatus;
PFN_vkSetEvent vulkanSymbolWrapper_vkSetEvent;
PFN_vkResetEvent vulkanSymbolWrapper_vkResetEvent;
PFN_vkCreateQueryPool vulkanSymbolWrapper_vkCreateQueryPool;
PFN_vkDestroyQueryPool vulkanSymbolWrapper_vkDestroyQueryPool;
PFN_vkGetQueryPoolResults vulkanSymbolWrapper_vkGetQueryPoolResults;
PFN_vkCreateBuffer vulkanSymbolWrapper_vkCreateBuffer;
PFN_vkDestroyBuffer vulkanSymbolWrapper_vkDestroyBuffer;
PFN_vkCreateBufferView vulkanSymbolWrapper_vkCreateBufferView;
PFN_vkDestroyBufferView vulkanSymbolWrapper_vkDestroyBufferView;
PFN_vkCreateImage vulkanSymbolWrapper_vkCreateImage;
PFN_vkDestroyImage vulkanSymbolWrapper_vkDestroyImage;
PFN_vkGetImageSubresourceLayout vulkanSymbolWrapper_vkGetImageSubresourceLayout;
PFN_vkCreateImageView vulkanSymbolWrapper_vkCreateImageView;
PFN_vkDestroyImageView vulkanSymbolWrapper_vkDestroyImageView;
PFN_vkCreateShaderModule vulkanSymbolWrapper_vkCreateShaderModule;
PFN_vkDestroyShaderModule vulkanSymbolWrapper_vkDestroyShaderModule;
PFN_vkCreatePipelineCache vulkanSymbolWrapper_vkCreatePipelineCache;
PFN_vkDestroyPipelineCache vulkanSymbolWrapper_vkDestroyPipelineCache;
PFN_vkGetPipelineCacheData vulkanSymbolWrapper_vkGetPipelineCacheData;
PFN_vkMergePipelineCaches vulkanSymbolWrapper_vkMergePipelineCaches;
PFN_vkCreateGraphicsPipelines vulkanSymbolWrapper_vkCreateGraphicsPipelines;
PFN_vkCreateComputePipelines vulkanSymbolWrapper_vkCreateComputePipelines;
PFN_vkDestroyPipeline vulkanSymbolWrapper_vkDestroyPipeline;
PFN_vkCreatePipelineLayout vulkanSymbolWrapper_vkCreatePipelineLayout;
PFN_vkDestroyPipelineLayout vulkanSymbolWrapper_vkDestroyPipelineLayout;
PFN_vkCreateSampler vulkanSymbolWrapper_vkCreateSampler;
PFN_vkDestroySampler vulkanSymbolWrapper_vkDestroySampler;
PFN_vkCreateDescriptorSetLayout vulkanSymbolWrapper_vkCreateDescriptorSetLayout;
PFN_vkDestroyDescriptorSetLayout vulkanSymbolWrapper_vkDestroyDescriptorSetLayout;
PFN_vkCreateDescriptorPool vulkanSymbolWrapper_vkCreateDescriptorPool;
PFN_vkDestroyDescriptorPool vulkanSymbolWrapper_vkDestroyDescriptorPool;
PFN_vkResetDescriptorPool vulkanSymbolWrapper_vkResetDescriptorPool;
PFN_vkAllocateDescriptorSets vulkanSymbolWrapper_vkAllocateDescriptorSets;
PFN_vkFreeDescriptorSets vulkanSymbolWrapper_vkFreeDescriptorSets;
PFN_vkUpdateDescriptorSets vulkanSymbolWrapper_vkUpdateDescriptorSets;
PFN_vkCreateFramebuffer vulkanSymbolWrapper_vkCreateFramebuffer;
PFN_vkDestroyFramebuffer vulkanSymbolWrapper_vkDestroyFramebuffer;
PFN_vkCreateRenderPass vulkanSymbolWrapper_vkCreateRenderPass;
PFN_vkDestroyRenderPass vulkanSymbolWrapper_vkDestroyRenderPass;
PFN_vkGetRenderAreaGranularity vulkanSymbolWrapper_vkGetRenderAreaGranularity;
PFN_vkCreateCommandPool vulkanSymbolWrapper_vkCreateCommandPool;
PFN_vkDestroyCommandPool vulkanSymbolWrapper_vkDestroyCommandPool;
PFN_vkResetCommandPool vulkanSymbolWrapper_vkResetCommandPool;
PFN_vkAllocateCommandBuffers vulkanSymbolWrapper_vkAllocateCommandBuffers;
PFN_vkFreeCommandBuffers vulkanSymbolWrapper_vkFreeCommandBuffers;
PFN_vkBeginCommandBuffer vulkanSymbolWrapper_vkBeginCommandBuffer;
PFN_vkEndCommandBuffer vulkanSymbolWrapper_vkEndCommandBuffer;
PFN_vkResetCommandBuffer vulkanSymbolWrapper_vkResetCommandBuffer;
PFN_vkCmdBindPipeline vulkanSymbolWrapper_vkCmdBindPipeline;
PFN_vkCmdSetViewport vulkanSymbolWrapper_vkCmdSetViewport;
PFN_vkCmdSetScissor vulkanSymbolWrapper_vkCmdSetScissor;
PFN_vkCmdSetLineWidth vulkanSymbolWrapper_vkCmdSetLineWidth;
PFN_vkCmdSetDepthBias vulkanSymbolWrapper_vkCmdSetDepthBias;
PFN_vkCmdSetBlendConstants vulkanSymbolWrapper_vkCmdSetBlendConstants;
PFN_vkCmdSetDepthBounds vulkanSymbolWrapper_vkCmdSetDepthBounds;
PFN_vkCmdSetStencilCompareMask vulkanSymbolWrapper_vkCmdSetStencilCompareMask;
PFN_vkCmdSetStencilWriteMask vulkanSymbolWrapper_vkCmdSetStencilWriteMask;
PFN_vkCmdSetStencilReference vulkanSymbolWrapper_vkCmdSetStencilReference;
PFN_vkCmdBindDescriptorSets vulkanSymbolWrapper_vkCmdBindDescriptorSets;
PFN_vkCmdBindIndexBuffer vulkanSymbolWrapper_vkCmdBindIndexBuffer;
PFN_vkCmdBindVertexBuffers vulkanSymbolWrapper_vkCmdBindVertexBuffers;
PFN_vkCmdDraw vulkanSymbolWrapper_vkCmdDraw;
PFN_vkCmdDrawIndexed vulkanSymbolWrapper_vkCmdDrawIndexed;
PFN_vkCmdDrawIndirect vulkanSymbolWrapper_vkCmdDrawIndirect;
PFN_vkCmdDrawIndexedIndirect vulkanSymbolWrapper_vkCmdDrawIndexedIndirect;
PFN_vkCmdDispatch vulkanSymbolWrapper_vkCmdDispatch;
PFN_vkCmdDispatchIndirect vulkanSymbolWrapper_vkCmdDispatchIndirect;
PFN_vkCmdCopyBuffer vulkanSymbolWrapper_vkCmdCopyBuffer;
PFN_vkCmdCopyImage vulkanSymbolWrapper_vkCmdCopyImage;
PFN_vkCmdBlitImage vulkanSymbolWrapper_vkCmdBlitImage;
PFN_vkCmdCopyBufferToImage vulkanSymbolWrapper_vkCmdCopyBufferToImage;
PFN_vkCmdCopyImageToBuffer vulkanSymbolWrapper_vkCmdCopyImageToBuffer;
PFN_vkCmdUpdateBuffer vulkanSymbolWrapper_vkCmdUpdateBuffer;
PFN_vkCmdFillBuffer vulkanSymbolWrapper_vkCmdFillBuffer;
PFN_vkCmdClearColorImage vulkanSymbolWrapper_vkCmdClearColorImage;
PFN_vkCmdClearDepthStencilImage vulkanSymbolWrapper_vkCmdClearDepthStencilImage;
PFN_vkCmdClearAttachments vulkanSymbolWrapper_vkCmdClearAttachments;
PFN_vkCmdResolveImage vulkanSymbolWrapper_vkCmdResolveImage;
PFN_vkCmdSetEvent vulkanSymbolWrapper_vkCmdSetEvent;
PFN_vkCmdResetEvent vulkanSymbolWrapper_vkCmdResetEvent;
PFN_vkCmdWaitEvents vulkanSymbolWrapper_vkCmdWaitEvents;
PFN_vkCmdPipelineBarrier vulkanSymbolWrapper_vkCmdPipelineBarrier;
PFN_vkCmdBeginQuery vulkanSymbolWrapper_vkCmdBeginQuery;
PFN_vkCmdEndQuery vulkanSymbolWrapper_vkCmdEndQuery;
PFN_vkCmdResetQueryPool vulkanSymbolWrapper_vkCmdResetQueryPool;
PFN_vkCmdWriteTimestamp vulkanSymbolWrapper_vkCmdWriteTimestamp;
PFN_vkCmdCopyQueryPoolResults vulkanSymbolWrapper_vkCmdCopyQueryPoolResults;
PFN_vkCmdPushConstants vulkanSymbolWrapper_vkCmdPushConstants;
PFN_vkCmdBeginRenderPass vulkanSymbolWrapper_vkCmdBeginRenderPass;
PFN_vkCmdNextSubpass vulkanSymbolWrapper_vkCmdNextSubpass;
PFN_vkCmdEndRenderPass vulkanSymbolWrapper_vkCmdEndRenderPass;
PFN_vkCmdExecuteCommands vulkanSymbolWrapper_vkCmdExecuteCommands;
PFN_vkDestroySurfaceKHR vulkanSymbolWrapper_vkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR vulkanSymbolWrapper_vkGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vulkanSymbolWrapper_vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vulkanSymbolWrapper_vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vulkanSymbolWrapper_vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkCreateSwapchainKHR vulkanSymbolWrapper_vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR vulkanSymbolWrapper_vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR vulkanSymbolWrapper_vkGetSwapchainImagesKHR;
PFN_vkAcquireNextImageKHR vulkanSymbolWrapper_vkAcquireNextImageKHR;
PFN_vkQueuePresentKHR vulkanSymbolWrapper_vkQueuePresentKHR;
PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vulkanSymbolWrapper_vkGetPhysicalDeviceDisplayPropertiesKHR;
PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vulkanSymbolWrapper_vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
PFN_vkGetDisplayPlaneSupportedDisplaysKHR vulkanSymbolWrapper_vkGetDisplayPlaneSupportedDisplaysKHR;
PFN_vkGetDisplayModePropertiesKHR vulkanSymbolWrapper_vkGetDisplayModePropertiesKHR;
PFN_vkCreateDisplayModeKHR vulkanSymbolWrapper_vkCreateDisplayModeKHR;
PFN_vkGetDisplayPlaneCapabilitiesKHR vulkanSymbolWrapper_vkGetDisplayPlaneCapabilitiesKHR;
PFN_vkCreateDisplayPlaneSurfaceKHR vulkanSymbolWrapper_vkCreateDisplayPlaneSurfaceKHR;
PFN_vkCreateSharedSwapchainsKHR vulkanSymbolWrapper_vkCreateSharedSwapchainsKHR;
PFN_vkCreateDebugReportCallbackEXT vulkanSymbolWrapper_vkCreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT vulkanSymbolWrapper_vkDestroyDebugReportCallbackEXT;
PFN_vkDebugReportMessageEXT vulkanSymbolWrapper_vkDebugReportMessageEXT;
PFN_vkDebugMarkerSetObjectTagEXT vulkanSymbolWrapper_vkDebugMarkerSetObjectTagEXT;
PFN_vkDebugMarkerSetObjectNameEXT vulkanSymbolWrapper_vkDebugMarkerSetObjectNameEXT;
PFN_vkCmdDebugMarkerBeginEXT vulkanSymbolWrapper_vkCmdDebugMarkerBeginEXT;
PFN_vkCmdDebugMarkerEndEXT vulkanSymbolWrapper_vkCmdDebugMarkerEndEXT;
PFN_vkCmdDebugMarkerInsertEXT vulkanSymbolWrapper_vkCmdDebugMarkerInsertEXT;
PFN_vkCmdDrawIndirectCountAMD vulkanSymbolWrapper_vkCmdDrawIndirectCountAMD;
PFN_vkCmdDrawIndexedIndirectCountAMD vulkanSymbolWrapper_vkCmdDrawIndexedIndirectCountAMD;
PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vulkanSymbolWrapper_vkGetPhysicalDeviceExternalImageFormatPropertiesNV;

#ifndef _WIN32
#include <dlfcn.h>
static void *dylib;
#endif

VkBool32 vulkanSymbolWrapperInitLoader()
{
#ifndef _WIN32
    if (!dylib)
    {
        dylib = dlopen("libvulkan.so", RTLD_LOCAL | RTLD_NOW);
    }

    if (dylib)
    {
        auto gpa = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(dylib, "vkGetInstanceProcAddr"));
        if (gpa)
            vulkanSymbolWrapperInit(gpa);
        else
            return VK_FALSE;
    }

    return dylib != nullptr ? VK_TRUE : VK_FALSE;
#else
    return VK_FALSE;
#endif
}

void vulkanSymbolWrapperUnload()
{
    dlclose(dylib);
}

static PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
void vulkanSymbolWrapperInit(PFN_vkGetInstanceProcAddr getInstanceProcAddr)
{
    GetInstanceProcAddr = getInstanceProcAddr;
}

VkBool32 vulkanSymbolWrapperLoadGlobalSymbols()
{
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(nullptr, "vkCreateInstance", vkCreateInstance))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(nullptr, "vkEnumerateInstanceExtensionProperties", vkEnumerateInstanceExtensionProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(nullptr, "vkEnumerateInstanceLayerProperties", vkEnumerateInstanceLayerProperties))
        return VK_FALSE;
    return VK_TRUE;
}

VkBool32 vulkanSymbolWrapperLoadInstanceSymbol(VkInstance instance, const char *name, PFN_vkVoidFunction *ppSymbol)
{
    *ppSymbol = GetInstanceProcAddr(instance, name);
    return *ppSymbol != NULL;
}

VkBool32 vulkanSymbolWrapperLoadCoreInstanceSymbols(VkInstance instance)
{
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkDestroyInstance", vkDestroyInstance))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkEnumeratePhysicalDevices", vkEnumeratePhysicalDevices))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetPhysicalDeviceFeatures", vkGetPhysicalDeviceFeatures))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetPhysicalDeviceFormatProperties", vkGetPhysicalDeviceFormatProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetPhysicalDeviceImageFormatProperties", vkGetPhysicalDeviceImageFormatProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetPhysicalDeviceProperties", vkGetPhysicalDeviceProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetPhysicalDeviceQueueFamilyProperties", vkGetPhysicalDeviceQueueFamilyProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetPhysicalDeviceMemoryProperties", vkGetPhysicalDeviceMemoryProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetDeviceProcAddr", vkGetDeviceProcAddr))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkCreateDevice", vkCreateDevice))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkEnumerateDeviceExtensionProperties", vkEnumerateDeviceExtensionProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkEnumerateDeviceLayerProperties", vkEnumerateDeviceLayerProperties))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkGetPhysicalDeviceSparseImageFormatProperties", vkGetPhysicalDeviceSparseImageFormatProperties))
        return VK_FALSE;
    return VK_TRUE;
}

VkBool32 vulkanSymbolWrapperLoadDeviceSymbol(VkDevice device, const char *name, PFN_vkVoidFunction *ppSymbol)
{
    *ppSymbol = vkGetDeviceProcAddr(device, name);
    return *ppSymbol != NULL;
}

VkBool32 vulkanSymbolWrapperLoadCoreDeviceSymbols(VkDevice device)
{
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyDevice", vkDestroyDevice))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetDeviceQueue", vkGetDeviceQueue))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkQueueSubmit", vkQueueSubmit))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkQueueWaitIdle", vkQueueWaitIdle))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDeviceWaitIdle", vkDeviceWaitIdle))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkAllocateMemory", vkAllocateMemory))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkFreeMemory", vkFreeMemory))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkMapMemory", vkMapMemory))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkUnmapMemory", vkUnmapMemory))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkFlushMappedMemoryRanges", vkFlushMappedMemoryRanges))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkInvalidateMappedMemoryRanges", vkInvalidateMappedMemoryRanges))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetDeviceMemoryCommitment", vkGetDeviceMemoryCommitment))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkBindBufferMemory", vkBindBufferMemory))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkBindImageMemory", vkBindImageMemory))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetBufferMemoryRequirements", vkGetBufferMemoryRequirements))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetImageMemoryRequirements", vkGetImageMemoryRequirements))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetImageSparseMemoryRequirements", vkGetImageSparseMemoryRequirements))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkQueueBindSparse", vkQueueBindSparse))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateFence", vkCreateFence))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyFence", vkDestroyFence))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkResetFences", vkResetFences))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetFenceStatus", vkGetFenceStatus))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkWaitForFences", vkWaitForFences))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateSemaphore", vkCreateSemaphore))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroySemaphore", vkDestroySemaphore))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateEvent", vkCreateEvent))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyEvent", vkDestroyEvent))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetEventStatus", vkGetEventStatus))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkSetEvent", vkSetEvent))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkResetEvent", vkResetEvent))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateQueryPool", vkCreateQueryPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyQueryPool", vkDestroyQueryPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetQueryPoolResults", vkGetQueryPoolResults))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateBuffer", vkCreateBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyBuffer", vkDestroyBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateBufferView", vkCreateBufferView))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyBufferView", vkDestroyBufferView))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateImage", vkCreateImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyImage", vkDestroyImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetImageSubresourceLayout", vkGetImageSubresourceLayout))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateImageView", vkCreateImageView))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyImageView", vkDestroyImageView))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateShaderModule", vkCreateShaderModule))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyShaderModule", vkDestroyShaderModule))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreatePipelineCache", vkCreatePipelineCache))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyPipelineCache", vkDestroyPipelineCache))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetPipelineCacheData", vkGetPipelineCacheData))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkMergePipelineCaches", vkMergePipelineCaches))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateGraphicsPipelines", vkCreateGraphicsPipelines))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateComputePipelines", vkCreateComputePipelines))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyPipeline", vkDestroyPipeline))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreatePipelineLayout", vkCreatePipelineLayout))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyPipelineLayout", vkDestroyPipelineLayout))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateSampler", vkCreateSampler))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroySampler", vkDestroySampler))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateDescriptorSetLayout", vkCreateDescriptorSetLayout))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyDescriptorSetLayout", vkDestroyDescriptorSetLayout))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateDescriptorPool", vkCreateDescriptorPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyDescriptorPool", vkDestroyDescriptorPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkResetDescriptorPool", vkResetDescriptorPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkAllocateDescriptorSets", vkAllocateDescriptorSets))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkFreeDescriptorSets", vkFreeDescriptorSets))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkUpdateDescriptorSets", vkUpdateDescriptorSets))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateFramebuffer", vkCreateFramebuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyFramebuffer", vkDestroyFramebuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateRenderPass", vkCreateRenderPass))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyRenderPass", vkDestroyRenderPass))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkGetRenderAreaGranularity", vkGetRenderAreaGranularity))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCreateCommandPool", vkCreateCommandPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkDestroyCommandPool", vkDestroyCommandPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkResetCommandPool", vkResetCommandPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkAllocateCommandBuffers", vkAllocateCommandBuffers))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkFreeCommandBuffers", vkFreeCommandBuffers))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkBeginCommandBuffer", vkBeginCommandBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkEndCommandBuffer", vkEndCommandBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkResetCommandBuffer", vkResetCommandBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdBindPipeline", vkCmdBindPipeline))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetViewport", vkCmdSetViewport))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetScissor", vkCmdSetScissor))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetLineWidth", vkCmdSetLineWidth))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetDepthBias", vkCmdSetDepthBias))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetBlendConstants", vkCmdSetBlendConstants))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetDepthBounds", vkCmdSetDepthBounds))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetStencilCompareMask", vkCmdSetStencilCompareMask))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetStencilWriteMask", vkCmdSetStencilWriteMask))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetStencilReference", vkCmdSetStencilReference))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdBindDescriptorSets", vkCmdBindDescriptorSets))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdBindIndexBuffer", vkCmdBindIndexBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdBindVertexBuffers", vkCmdBindVertexBuffers))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdDraw", vkCmdDraw))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdDrawIndexed", vkCmdDrawIndexed))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdDrawIndirect", vkCmdDrawIndirect))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdDrawIndexedIndirect", vkCmdDrawIndexedIndirect))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdDispatch", vkCmdDispatch))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdDispatchIndirect", vkCmdDispatchIndirect))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdCopyBuffer", vkCmdCopyBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdCopyImage", vkCmdCopyImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdBlitImage", vkCmdBlitImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdCopyBufferToImage", vkCmdCopyBufferToImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdCopyImageToBuffer", vkCmdCopyImageToBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdUpdateBuffer", vkCmdUpdateBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdFillBuffer", vkCmdFillBuffer))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdClearColorImage", vkCmdClearColorImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdClearDepthStencilImage", vkCmdClearDepthStencilImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdClearAttachments", vkCmdClearAttachments))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdResolveImage", vkCmdResolveImage))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdSetEvent", vkCmdSetEvent))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdResetEvent", vkCmdResetEvent))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdWaitEvents", vkCmdWaitEvents))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdPipelineBarrier", vkCmdPipelineBarrier))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdBeginQuery", vkCmdBeginQuery))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdEndQuery", vkCmdEndQuery))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdResetQueryPool", vkCmdResetQueryPool))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdWriteTimestamp", vkCmdWriteTimestamp))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdCopyQueryPoolResults", vkCmdCopyQueryPoolResults))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdPushConstants", vkCmdPushConstants))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdBeginRenderPass", vkCmdBeginRenderPass))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdNextSubpass", vkCmdNextSubpass))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdEndRenderPass", vkCmdEndRenderPass))
        return VK_FALSE;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_DEVICE_SYMBOL(device, "vkCmdExecuteCommands", vkCmdExecuteCommands))
        return VK_FALSE;
    return VK_TRUE;
}