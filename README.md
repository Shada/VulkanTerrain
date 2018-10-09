# VulkanTerrain
Another vulkan test project. 


I have been creating many Vulkan test projects but never really have finished any.

I hope this will be one where I acctually get some game or demo out of. I will try to incorporate ideas from my other projects here.

Only supports linux for now, but I will make sure to make it cross-platform eventually. 

Build using gcc :

g++ VulkanCore.cpp WindowXcb.cpp main.cpp -I$VULKAN_SDK/include -L$VULKAN_SDK/lib -lxcb -lvulkan -DVK_USE_PLATFORM_XCB_KHR -LGLSLANG_PATH/include -LGLSLANG_PATH/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools

I will add makefile/cmake in due time. 

I will also add external libs as submodules to simplify building etc


The Vulkan-parts are in massive need of a refactoring, but the current priorities is to get a simple demo running, and then refactor from there. 

I also need to add tests, and I will add validation layers for vulkan too.
