#pragma once

#include <memory>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "WindowXcb.hpp"

namespace Tobi
{

class Camera
{
  public:
    Camera(std::shared_ptr<WindowXcb> window);
    Camera(const Camera &) = delete;
    Camera(Camera &&) = default;
    Camera &operator=(const Camera &) & = delete;
    Camera &operator=(Camera &&) & = default;
    ~Camera() = default;

    const glm::mat4 &getModelViewProjectionMatrix() { return modelViewProjectionMatrix; }

  private:
    void initCamera();

    std::shared_ptr<WindowXcb> window;

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