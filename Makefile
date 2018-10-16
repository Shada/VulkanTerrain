CC=gcc
CXX=g++
CXXFLAGS=-I. -I$(VULKAN_SDK)/include -I/home/admin/Documents/Programming/vulkan/glslang/install/include -I./Utils -DVK_USE_PLATFORM_XCB_KHR 

UTILFILES=Utils/glslanghelper.hpp
PIPEFILES=Pipeline/VulkanPipeline.hpp Pipeline/VulkanPipelineCache.hpp 
BUFFILES=Buffers/VulkanVertexBuffer.hpp Buffers/VulkanFrameBuffers.hpp Buffers/VulkanUniformBuffer.hpp Buffers/VulkanDepthBuffer.hpp 
BASEFILES=Camera.hpp VulkanRenderPass.hpp VulkanDescriptorPool.hpp VulkanSwapChain.hpp VulkanCore.hpp WindowXcb.hpp 
CMDFILES=Command/VulkanCommandBuffer.hpp Command/VulkanCommandPool.hpp 
SDRFILES=Shader/VulkanShaderProgram.hpp 
DEPS=$(BASEFILES) $(UTILFILES) $(PIPEFILES) $(BUFFILES) $(CMDFILES)

UTILOBJ=obj/Utils/glslanghelper.o 
PIPEOBJ=obj/Pipeline/VulkanPipeline.o obj/Pipeline/VulkanPipelineCache.o 
BUFOBJ=obj/Buffers/VulkanVertexBuffer.o obj/Buffers/VulkanFrameBuffers.o obj/Buffers/VulkanUniformBuffer.o obj/Buffers/VulkanDepthBuffer.o 
BASEOBJ=obj/Camera.o obj/VulkanRenderPass.o obj/VulkanDescriptorPool.o obj/VulkanSwapChain.o obj/VulkanCore.o obj/WindowXcb.o 
CMDOBJ=obj/Command/VulkanCommandBuffer.o obj/Command/VulkanCommandPool.o 
SDROBJ=obj/Shader/VulkanShaderProgram.o 
OBJ=$(BASEOBJ) $(UTILOBJ) $(PIPEOBJ) $(BUFOBJ) $(CMDOBJ) $(SDROBJ)

obj/%.o: %.cpp $(DEPS)
	$(CXX) -c -o$@ $< $(CXXFLAGS) -std=c++17 -g 

TobiGame.out: $(OBJ)
	$(CXX) -o $@ $^ -std=c++17 -g main.cpp -I$(VULKAN_SDK)/include -L$(VULKAN_SDK)/lib -lxcb -lvulkan -lpthread -L/home/admin/Documents/Programming/vulkan/glslang/install/lib -lglslang -lHLSL -lSPIRV -lSPIRV-Tools-opt -lSPIRV-Tools
