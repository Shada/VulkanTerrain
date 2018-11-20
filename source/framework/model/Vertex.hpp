#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Tobi
{

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 colour;
};

} // namespace Tobi