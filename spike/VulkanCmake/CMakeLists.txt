cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
 
project (Test)

## Uncomment these when you want to do testing in the install dir, 
## without the need of editing LD_LIBRARY_PATH 
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
 
include_directories (include)
include_directories (src)
include_directories (external)

add_subdirectory (src) 

install (DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install (DIRECTORY assets DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install (DIRECTORY assets DESTINATION ${CMAKE_INSTALL_PREFIX}/tests)

add_subdirectory (tests) 