# VulkanTerrain
Another vulkan test project. 


I have been creating many Vulkan test projects but never really have finished any.

I hope this will be one where I acctually get some game or demo out of. I will try to incorporate ideas from my other projects here.

Only supports linux for now, but I will make sure to make it cross-platform eventually. 

Build using make:
    make TobiGame.out

TODO:
* Add cmake build. 
* Add glslang (and other future external libs) as submodules 
* Refactor vulkan-parts (mostly done, but still too much in vulkan stuff in context)
* Add unit tests
* Load models from file
* Iterate through objects when rendering/updating
* Make scene that can hold collection of objects
* Make option to load shaders with glslang 
* Extract descriptor info from shaders
* Make camera rotatable
* Better key mapping