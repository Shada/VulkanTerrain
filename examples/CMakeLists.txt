
add_executable(tobiex main.cpp)
target_link_libraries(tobiex PUBLIC tobi)

install (TARGETS tobiex 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/examples
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)


add_executable(gametest gametest.cpp)
target_link_libraries(gametest PUBLIC tobi)

install (TARGETS gametest 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/examples
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    
    
install (DIRECTORY ../assets DESTINATION ${CMAKE_INSTALL_PREFIX}/examples)
