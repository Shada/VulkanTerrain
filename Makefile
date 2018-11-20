CC=gcc
CXX=g++
CXXFLAGS=-Isource -Isource/game -Isource/platform -Isource/framework -I$(VULKAN_SDK)/include -I/home/admin/Documents/Programming/vulkan/glslang/install/include -DVK_USE_PLATFORM_XCB_KHR 

PFP = source/platform/
FWP = source/framework/
GFP = source/game/

PLATFORMFILES = $(PFP)Platform.hpp $(PFP)xcb/PlatformXcb.hpp $(PFP)AssetManager.hpp
FRAMEWORKFILES = $(FWP)Context.hpp $(FWP)SemaphoreManager.hpp $(FWP)PerFrame.hpp \
	$(FWP)CommandBufferManager.hpp $(FWP)FenceManager.hpp $(FWP)buffers/VertexBufferManager.hpp \
	$(FWP)buffers/UniformBufferManager.hpp $(FWP)buffers/IndexBufferManager.hpp \
	$(FWP)buffers/BufferManager.hpp $(FWP)model/Model.hpp \
	$(FWP)model/ModelManager.hpp $(FWP)model/ObjectManager.hpp $(FWP)EventDispatchers.hpp
GAMEFILES = $(GFP)KeyState.hpp $(GFP)Camera.hpp
BASEFILES = source/libvulkan-loader.hpp 
DEPS=$(BASEFILES) $(GAMEFILES) $(FRAMEWORKFILES) $(PLATFORMFILES)

PLATFORMOBJ = obj/$(PFP)Platform.o obj/$(PFP)xcb/PlatformXcb.o obj/$(PFP)AssetManager.o
FRAMEWORKOBJ = obj/$(FWP)Context.o obj/$(FWP)SemaphoreManager.o obj/$(FWP)PerFrame.o \
	obj/$(FWP)CommandBufferManager.o obj/$(FWP)FenceManager.o obj/$(FWP)buffers/VertexBufferManager.o \
	obj/$(FWP)buffers/UniformBufferManager.o obj/$(FWP)buffers/IndexBufferManager.o \
	obj/$(FWP)buffers/BufferManager.o obj/$(FWP)model/Model.o \
	obj/$(FWP)model/ModelManager.o obj/$(FWP)model/ObjectManager.o obj/$(FWP)EventDispatchers.o
GAMEOBJ = obj/$(GFP)KeyState.o obj/$(GFP)Camera.o
BASEOBJ = obj/source/libvulkan-loader.o 
OBJ=$(BASEOBJ) $(PLATFORMOBJ) $(GAMEOBJ) $(FRAMEWORKOBJ)

obj/%.o: %.cpp $(DEPS)
	$(CXX) -c -g -o$@ $< $(CXXFLAGS) -std=c++17 

TobiGame.out: $(OBJ)
	$(CXX) -o $@ $^ -g -std=c++17 source/main.cpp -I$(VULKAN_SDK)/include -L$(VULKAN_SDK)/lib \
		-ldl -lxcb -lX11 -lX11-xcb -lxcb-util -lvulkan -lpthread -L/home/admin/Documents/Programming/vulkan/glslang/install/lib \
		-lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools -lassimp
