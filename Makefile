CC=gcc
CXX=g++
CXXFLAGS=-Isource/. -Isource/platform -Isource/framework -I$(VULKAN_SDK)/include -I/home/admin/Documents/Programming/vulkan/glslang/install/include -DVK_USE_PLATFORM_XCB_KHR 

PFP = source/platform/
FWP = source/framework/

PLATFORMFILES = $(PFP)Platform.hpp $(PFP)xcb/PlatformXcb.hpp $(PFP)AssetManager.hpp
FRAMEWORKFILES = $(FWP)Context.hpp $(FWP)CommandBufferManager.hpp $(FWP)FenceManager.hpp \
	$(FWP)SemaphoreManager.hpp $(FWP)model/Model.hpp $(FWP)buffers/VertexBufferManager.hpp \
	$(FWP)buffers/BufferManager.hpp $(FWP)PerFrame.hpp
	
BASEFILES =  source/libvulkan-loader.hpp
DEPS=$(BASEFILES) $(UTILFILES) $(PIPEFILES) $(BUFFILES) $(CMDFILES)

PLATFORMFILES = obj/$(PFP)Platform.o obj/$(PFP)xcb/PlatformXcb.o obj/$(PFP)AssetManager.o
FRAMEWORKFILES = obj/$(FWP)Context.o obj/$(FWP)CommandBufferManager.o obj/$(FWP)FenceManager.o \
	obj/$(FWP)SemaphoreManager.o  obj/$(FWP)model/Model.o obj/$(FWP)buffers/VertexBufferManager.o \
	obj/$(FWP)buffers/BufferManager.o obj/$(FWP)PerFrame.o
BASEOBJ = obj/source/libvulkan-loader.o
OBJ=$(BASEOBJ) $(PLATFORMFILES) $(FRAMEWORKFILES)

obj/%.o: %.cpp $(DEPS)
	$(CXX) -c -g -o$@ $< $(CXXFLAGS) -std=c++17 

TobiGame.out: $(OBJ)
	$(CXX) -o $@ $^ -g -std=c++17  source/main.cpp -I$(VULKAN_SDK)/include -L$(VULKAN_SDK)/lib -ldl -lxcb -lxcb-util -lvulkan -lpthread -L/home/admin/Documents/Programming/vulkan/glslang/install/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools
