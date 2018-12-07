file(
    GLOB_RECURSE
    UNIT_TEST_SOURCES
    ./*
)

add_executable(UnitTests ${UNIT_TEST_SOURCES})

target_link_libraries(UnitTests PUBLIC tobi)
target_include_directories(UnitTests PUBLIC "../external/catch2")

#link_directories (${CMAKE_INSTALL_PREFIX}/lib)

file (COPY ../assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install (TARGETS UnitTests 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/tests
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install (DIRECTORY ../assets DESTINATION ${CMAKE_INSTALL_PREFIX}/tests)
