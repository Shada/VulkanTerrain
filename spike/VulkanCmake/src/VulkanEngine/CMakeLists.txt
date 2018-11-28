file(
    GLOB_RECURSE
    VULKAN_ENGINE_SOURCES
    ./*
)

add_library(VulkanEngine SHARED ${VULKAN_ENGINE_SOURCES})

target_link_libraries (VulkanEngine assimp ${Vulkan_LIBRARY})

install (TARGETS VulkanEngine DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)