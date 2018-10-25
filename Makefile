CC=gcc
CXX=g++
CXXFLAGS=-Isource/. -Isource/platform -Isource/framework -I$(VULKAN_SDK)/include -I/home/admin/Documents/Programming/vulkan/glslang/install/include -DVK_USE_PLATFORM_XCB_KHR 

PLATFORMFILES = source/platform/Platform.hpp source/platform/xcb/PlatformXcb.hpp source/platform/AssetManager.hpp
FRAMEWORKFILES = source/framework/Context.hpp source/framework/CommandBufferManager.hpp source/framework/FenceManager.hpp \
	source/framework/SemaphoreManager.hpp source/framework/model/Model.hpp source/framework/buffers/VertexBufferManager.hpp source/framework/buffers/BufferManager.hpp 
	
BASEFILES =  source/libvulkan-loader.hpp
DEPS=$(BASEFILES) $(UTILFILES) $(PIPEFILES) $(BUFFILES) $(CMDFILES)

PLATFORMFILES = obj/source/platform/Platform.o obj/source/platform/xcb/PlatformXcb.o obj/source/platform/AssetManager.o
FRAMEWORKFILES = obj/source/framework/Context.o obj/source/framework/CommandBufferManager.o obj/source/framework/FenceManager.o \
	obj/source/framework/SemaphoreManager.o  obj/source/framework/model/Model.o obj/source/framework/buffers/VertexBufferManager.o obj/source/framework/buffers/BufferManager.o
BASEOBJ = obj/source/libvulkan-loader.o
OBJ=$(BASEOBJ) $(PLATFORMFILES) $(FRAMEWORKFILES)

obj/%.o: %.cpp $(DEPS)
	$(CXX) -c -g -o$@ $< $(CXXFLAGS) -std=c++17 -O3

TobiGame.out: $(OBJ)
	$(CXX) -o $@ $^ -g -std=c++17 -O3 source/main.cpp -I$(VULKAN_SDK)/include -L$(VULKAN_SDK)/lib -ldl -lxcb -lxcb-util -lvulkan -lpthread -L/home/admin/Documents/Programming/vulkan/glslang/install/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools
