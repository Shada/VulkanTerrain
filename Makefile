CC=gcc
CXX=g++
CXXFLAGS=-Isource/. -Isource/platform -Isource/framework -I$(VULKAN_SDK)/include -I/home/admin/Documents/Programming/vulkan/glslang/install/include -DVK_USE_PLATFORM_XCB_KHR 

PFP = source/platform/
FWP = source/framework/

PLATFORMFILES = $(PFP)Platform.hpp $(PFP)xcb/PlatformXcb.hpp $(PFP)AssetManager.hpp
FRAMEWORKFILES = $(FWP)Context.hpp $(FWP)SemaphoreManager.hpp $(FWP)PerFrame.hpp \
$(FWP)CommandBufferManager.hpp $(FWP)FenceManager.hpp $(FWP)buffers/VertexBufferManager.hpp \
	$(FWP)buffers/UniformBufferManager.hpp $(FWP)buffers/BufferManager.hpp $(FWP)model/Model.hpp \
	$(FWP)model/ModelManager.hpp $(FWP)model/ObjectManager.hpp source/Game/Camera.hpp
	
BASEFILES =  source/libvulkan-loader.hpp
DEPS=$(BASEFILES) $(UTILFILES) $(PIPEFILES) $(BUFFILES) $(CMDFILES)

PLATFORMFILES = obj/$(PFP)Platform.o obj/$(PFP)xcb/PlatformXcb.o obj/$(PFP)AssetManager.o
FRAMEWORKFILES = obj/$(FWP)Context.o obj/$(FWP)SemaphoreManager.o obj/$(FWP)PerFrame.o \
 obj/$(FWP)CommandBufferManager.o obj/$(FWP)FenceManager.o obj/$(FWP)buffers/VertexBufferManager.o \
	obj/$(FWP)buffers/UniformBufferManager.o obj/$(FWP)buffers/BufferManager.o obj/$(FWP)model/Model.o \
	obj/$(FWP)model/ModelManager.o obj/$(FWP)model/ObjectManager.o obj/source/Game/Camera.o
BASEOBJ = obj/source/libvulkan-loader.o
OBJ=$(BASEOBJ) $(PLATFORMFILES) $(FRAMEWORKFILES)

obj/%.o: %.cpp $(DEPS)
	$(CXX) -c -g -o$@ $< $(CXXFLAGS) -std=c++17 

TobiGame.out: $(OBJ)
	$(CXX) -o $@ $^ -g -std=c++17  source/main.cpp -I$(VULKAN_SDK)/include -L$(VULKAN_SDK)/lib -ldl -lxcb -lxcb-util -lvulkan -lpthread -L/home/admin/Documents/Programming/vulkan/glslang/install/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools
