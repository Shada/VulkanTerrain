#pragma once
#include <glm/glm.hpp>

namespace Tobi
{

struct ShaderDataBlock
{
    glm::mat4x4 modelMatrix;
    glm::mat4x4 viewProjectionMatrix;
};

}