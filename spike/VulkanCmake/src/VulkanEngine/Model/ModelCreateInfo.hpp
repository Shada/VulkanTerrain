#pragma once

// TODO: have glm in external,
#include <glm/glm.hpp>

namespace TobiEngine
{

/** @brief Used to parametrize model loading */
class ModelCreateInfo
{
  public:
    ModelCreateInfo();
    ModelCreateInfo(glm::vec3 scale, glm::vec2 uvscale, glm::vec3 center);
    ModelCreateInfo(float scale, float uvscale, float center);
    ~ModelCreateInfo() = default;

    glm::vec3 getCenter() { return center; }
    glm::vec3 getScale() { return scale; }
    glm::vec2 getUvscale() { return uvscale; }

  private:
    glm::vec3 center;
    glm::vec3 scale;
    glm::vec2 uvscale;
};

} // namespace TobiEngine