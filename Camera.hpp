#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "WindowSettings.hpp"

namespace Tobi
{

class Camera
{
  public:
    Camera(WindowSettings windowSettings);
    Camera(const Camera &) = delete;
    Camera(Camera &&) = default;
    Camera &operator=(const Camera &) & = delete;
    Camera &operator=(Camera &&) & = default;
    ~Camera() = default;

    const glm::mat4 &getModelViewProjectionMatrix() { return modelViewProjectionMatrix; }

  private:
    void initCamera();

    WindowSettings windowSettings;

    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    glm::mat4 clipMatrix;
    glm::mat4 modelViewProjectionMatrix;
};

} // namespace Tobi