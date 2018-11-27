#include "ModelCreateInfo.hpp"

namespace TobiEngine
{

ModelCreateInfo::ModelCreateInfo()
    : center(glm::vec3()),
      scale(glm::vec3()),
      uvscale(glm::vec3())
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