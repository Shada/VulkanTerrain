#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Tobi
{

bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv);
void initGlslang();
void finalizeGlslang();

} // namespace Tobi