# the minimum version of CMake required

set(SOURCES 
    tobi.cpp
    libvulkan-loader.cpp
    framework/CommandBufferManager.cpp
    framework/Context.cpp
    framework/EventDispatchers.cpp
    framework/FenceManager.cpp
    framework/IContext.cpp
    framework/PerFrame.cpp
    framework/SemaphoreManager.cpp
    framework/buffers/BufferManager.cpp
    framework/buffers/IndexBufferManager.cpp
    framework/buffers/UniformBufferManager.cpp
    framework/buffers/VertexBufferManager.cpp
    framework/model/Model.cpp
    framework/model/ModelManager.cpp
    framework/model/ObjectManager.cpp
    game/KeyState.cpp
    game/Camera.cpp
    platform/AssetManager.cpp
    platform/Platform.cpp
    platform/xcb/PlatformXcb.cpp)

find_package(Vulkan REQUIRED)

find_package(glm REQUIRED)

find_package(Assimp 4.1 REQUIRED)

IF(USE_D2D_WSI)
	MESSAGE("Using direct to display extension...")
	add_definitions(-D_DIRECT2DISPLAY)
ELSEIF(USE_WAYLAND_WSI)
	find_package(Wayland REQUIRED)
	if (NOT WAYLAND_FOUND)
		message(FATAL_ERROR "Wayland development package not found")
	endif ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR")
	include_directories(${WAYLAND_INCLUDE_DIR})
ELSE(USE_D2D_WSI)
    find_library(xcb REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR")
ENDIF(USE_D2D_WSI)


add_library(tobi SHARED ${SOURCES})
target_compile_options(tobi PRIVATE "-std=c++14")

target_link_libraries(tobi PUBLIC glm)
target_link_libraries(tobi PUBLIC xcb xcb-util)

if(${Assimp_FOUND})
    set(ASSIMP_LIBRARY "assimp")
    add_library(${ASSIMP_LIBRARY} SHARED IMPORTED)
    set_target_properties(${ASSIMP_LIBRARY} PROPERTIES IMPORTED_LOCATION "${ASSIMP_LIBRARY_DIRS}/libassimp.so")
    target_link_libraries(tobi PUBLIC ${ASSIMP_LIBRARY})
else()
    message("Please install assimp in order to build") 
endif()
    
    
if(${Vulkan_FOUND} AND NOT TARGET Vulkan::Vulkan)
	add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
	set_target_properties(Vulkan::Vulkan PROPERTIES
			IMPORTED_LOCATION "${Vulkan_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIR}")
    target_include_directories(tobi PUBLIC ${Vulkan_INCLUDE_DIR})
    target_link_libraries(tobi PUBLIC ${Vulkan_LIBRARY})
elseif(${Vulkan_FOUND})
	set_target_properties(Vulkan::Vulkan PROPERTIES
			IMPORTED_LOCATION "${Vulkan_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIR}")
    target_include_directories(tobi PUBLIC ${Vulkan_INCLUDE_DIR})
    target_link_libraries(tobi PUBLIC ${Vulkan_LIBRARY})
else()
    message("Please install vulkan in order to build") 
endif()

install (TARGETS tobi
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
