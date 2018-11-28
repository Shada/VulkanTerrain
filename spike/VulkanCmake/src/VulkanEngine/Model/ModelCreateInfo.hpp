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

    const auto &getCenter() const { return center; }
    const auto &getScale() const { return scale; }
    const auto &getUvscale() const { return uvscale; }

  private:
    glm::vec3 center;
    glm::vec3 scale;
    glm::vec2 uvscale;
};

} // namespace TobiEngine