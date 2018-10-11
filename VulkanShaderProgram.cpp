#include "VulkanShaderProgram.hpp"

#include "VertexShaderText.hpp"
#include "FragmentShaderText.hpp"
#include "util.hpp"

namespace Tobi
{
VulkanShaderProgram::VulkanShaderProgram(std::shared_ptr<WindowXcb> window)
    : window(window)
{
    initShaders();
}

VulkanShaderProgram::~VulkanShaderProgram()
{
    for (auto &shaderStage : shaderStages)
    {
        if (shaderStage.module)
        {
            vkDestroyShaderModule(window->getDevice(), shaderStage.module, nullptr);
        }
    }
}

void VulkanShaderProgram::initShaders()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;
    bool U_ASSERT_ONLY returnValue = true;

    // If no shaders were submitted, just return
    if (!(ShaderText::vertexShaderText || ShaderText::fragmentShaderText))
    {
        std::cout << "shaders not complete" << std::endl;
        return;
    }

    initGlslang();

    VkShaderModuleCreateInfo moduleCreateInfo;

    if (ShaderText::vertexShaderText)
    {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
        std::vector<unsigned int> vertexShaderSpv;
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.pNext = nullptr;
        shaderStageCreateInfo.pSpecializationInfo = nullptr;
        shaderStageCreateInfo.flags = 0;
        shaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStageCreateInfo.pName = "main";

        returnValue = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, ShaderText::vertexShaderText, vertexShaderSpv);
        assert(returnValue);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = nullptr;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = vertexShaderSpv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = vertexShaderSpv.data();
        result = vkCreateShaderModule(window->getDevice(), &moduleCreateInfo, nullptr, &shaderStageCreateInfo.module);
        assert(result == VK_SUCCESS);

        shaderStages.push_back(shaderStageCreateInfo);
    }

    if (ShaderText::fragmentShaderText)
    {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
        std::vector<unsigned int> fragmentShaderSpv;
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.pNext = nullptr;
        shaderStageCreateInfo.pSpecializationInfo = nullptr;
        shaderStageCreateInfo.flags = 0;
        shaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageCreateInfo.pName = "main";

        returnValue = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, ShaderText::fragmentShaderText, fragmentShaderSpv);
        assert(returnValue);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = nullptr;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = fragmentShaderSpv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = fragmentShaderSpv.data();
        result = vkCreateShaderModule(window->getDevice(), &moduleCreateInfo, nullptr, &shaderStageCreateInfo.module);
        assert(result == VK_SUCCESS);

        shaderStages.push_back(shaderStageCreateInfo);
    }

    finalizeGlslang();
}

} // namespace Tobi