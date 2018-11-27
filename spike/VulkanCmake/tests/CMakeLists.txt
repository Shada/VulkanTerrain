file(
    GLOB_RECURSE
    UNIT_TEST_SOURCES
    ./*
)

add_executable(UnitTests ${UNIT_TEST_SOURCES})

link_directories (${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries (UnitTests VulkanEngine)

install (TARGETS UnitTests DESTINATION ${CMAKE_INSTALL_PREFIX}/tests)