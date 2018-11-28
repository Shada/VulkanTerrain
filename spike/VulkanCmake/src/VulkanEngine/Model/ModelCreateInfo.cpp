#include "ModelCreateInfo.hpp"

namespace TobiEngine
{

ModelCreateInfo::ModelCreateInfo()
    : center(glm::vec3(0.f)),
      scale(glm::vec3(1.f)),
      uvscale(glm::vec3(1.f))
{
}

ModelCreateInfo::ModelCreateInfo(glm::vec3 scale, glm::vec2 uvscale, glm::vec3 center)
    : center(center),
      scale(scale),
      uvscale(uvscale)
{
}

ModelCreateInfo::ModelCreateInfo(float scale, float uvscale, float center)
    : center(glm::vec3(center)),
      scale(glm::vec3(scale)),
      uvscale(glm::vec2(uvscale))
{
}

} // namespace TobiEngine