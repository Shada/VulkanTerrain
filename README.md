# VulkanTerrain
Another vulkan test project. 


I have been creating many Vulkan test projects but never really have finished any.

I hope this will be one where I acctually get some game or demo out of. I will try to incorporate ideas from my other projects here.

Only supports linux for now, but I will make sure to make it cross-platform eventually. 

Build using gcc :

g++ VulkanCore.cpp WindowXcb.cpp main.cpp -I$VULKAN_SDK/include -L$VULKAN_SDK/lib -lxcb -lvulkan -DVK_USE_PLATFORM_XCB_KHR -LGLSLANG_PATH/include -LGLSLANG_PATH/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools


TODO:
* Add makefile/cmake. 
* Add glslang (and other future external libs) as submodules 
* Refactor Vulkan-init. 
* Add unit tests
* Add vulkan validation layers
* Make sure build speed is satisfactory
* Extract descriptor info from shaders
* Load models
* Load shaders from files
* Make game loop
* Implement input handling (start with ESC for exit)
* Create camera with movement
