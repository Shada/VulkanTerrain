# VulkanTerrain
Another vulkan test project. 


I have been creating many Vulkan test projects but never really have finished any.

I hope this will be one where I acctually get some game or demo out of. I will try to incorporate ideas from my other projects here.

Only supports linux for now, but I will make sure to make it cross-platform eventually. 

Build using make:
    make TobiGame.out

In this branch I perform a big refurbishment where I take ideas from other projects (such as github.com/ARM-Software/vulkan-sdk)
I modify the code to suit my code standards and my own project structure.


TODO:
* Add cmake. 
* Add glslang (and other future external libs) as submodules 
* Refactor vulkan-parts
* Improve performance
* Add unit tests
* Add vulkan validation layers
* Make sure build speed is satisfactory (it's ok now, but could be better)
* Extract descriptor info from shaders
* Load models
* Load shaders from files
* Make game loop more efficient (drawing is a bit slow)
* Create camera with movement
* Window resize is a bit slow as of now


check https://github.com/ARM-software/vulkan-sdk.git