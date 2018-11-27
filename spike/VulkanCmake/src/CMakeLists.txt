
file(
    GLOB
    APPLICATION_SOURCE_FILES
    ./*
)

add_subdirectory (MyMath) 
add_subdirectory (VulkanEngine) 

add_executable(${PROJECT_NAME} ${APPLICATION_SOURCE_FILES})

link_directories (${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries (${PROJECT_NAME} MathyMath VulkanEngine)

install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
