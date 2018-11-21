# VulkanTerrain
Another vulkan test project. 


I have been creating many Vulkan test projects but never really have finished any.

I hope this will be one where I acctually get some game or demo out of. I will try to incorporate ideas from my other projects here.

Only supports linux for now, but I will make sure to make it cross-platform eventually. 

Build using make:
    make TobiGame.out

TODO:
* More optimized rendering (use pre-build command buffer, etc)
* Refactor Context-class (separate pipeline etc)
* Add Windows-support
* Make scene that can hold collection of objects
* Better model / mesh / object management
* Add texture support to models
* Render from the renderable object itself
* Make camera rotatable (mouse input)
* Add GUI with debug info (fps, log, etc)
* Add cmake build. 
* Add glslang (and other future external libs) as submodules 
* Refactor vulkan-parts (mostly done, but still too much in vulkan stuff in context)
* Add unit tests
* Make option to load shaders with glslang 
* Extract descriptor info from shaders
* Window aspect ratio distorts the scene (update projection matrix after window resize?)
* Add instancing