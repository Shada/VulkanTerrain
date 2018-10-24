CC=gcc
CXX=g++
CXXFLAGS=-I. -Iplatform -Iframework -I$(VULKAN_SDK)/include -I/home/admin/Documents/Programming/vulkan/glslang/install/include -DVK_USE_PLATFORM_XCB_KHR 

PLATFORMFILES = platform/Platform.hpp platform/xcb/PlatformXcb.hpp
#FRAMEWORKFILES = framework/Context.hpp framework/CommandBufferManager.hpp framework/FenceManager.hpp framework/SemaphoreManager.hpp
BASEFILES =  libvulkan-loader.hpp
DEPS=$(BASEFILES) $(UTILFILES) $(PIPEFILES) $(BUFFILES) $(CMDFILES)

PLATFORMFILES = obj/platform/Platform.o obj/platform/xcb/PlatformXcb.o 
#FRAMEWORKFILES = obj/framework/Context.o obj/framework/CommandBufferManager.o obj/framework/FenceManager.o obj/framework/SemaphoreManager.o
BASEOBJ = obj/libvulkan-loader.o
OBJ=$(BASEOBJ) $(PLATFORMFILES) $(FRAMEWORKFILES)

obj/%.o: %.cpp $(DEPS)
	$(CXX) -c -g -o$@ $< $(CXXFLAGS) -std=c++17 -O3

TobiGame.out: $(OBJ)
	$(CXX) -o $@ $^ -g -std=c++17 -O3 main.cpp -I$(VULKAN_SDK)/include -L$(VULKAN_SDK)/lib -ldl -lxcb -lxcb-util -lvulkan -lpthread -L/home/admin/Documents/Programming/vulkan/glslang/install/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools
