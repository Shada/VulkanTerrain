CC=gcc
CXX=g++
CXXFLAGS=-I. -I./stub -I$(VULKAN_SDK)/include -I/home/admin/Documents/Programming/vulkan/glslang/install/include -DVK_USE_PLATFORM_XCB_KHR 

BASEFILES= Platform.hpp PlatformXcb.hpp libvulkan-loader.hpp
DEPS=$(BASEFILES) $(UTILFILES) $(PIPEFILES) $(BUFFILES) $(CMDFILES)

BASEOBJ= obj/Platform.o obj/PlatformXcb.o obj/libvulkan-loader.o
OBJ=$(BASEOBJ) $(UTILOBJ) $(PIPEOBJ) $(BUFOBJ) $(CMDOBJ) $(SDROBJ)

obj/%.o: %.cpp $(DEPS)
	$(CXX) -c -g -o$@ $< $(CXXFLAGS) -std=c++17 -O3

TobiGame.out: $(OBJ)
	$(CXX) -o $@ $^ -g -std=c++17 -O3 main.cpp -I$(VULKAN_SDK)/include -L$(VULKAN_SDK)/lib -ldl -lxcb -lxcb-util -lvulkan -lpthread -L/home/admin/Documents/Programming/vulkan/glslang/install/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools
