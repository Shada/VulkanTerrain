#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "../framework/Common.hpp"

#include "../platform/SwapChainDimensions.hpp"

namespace Tobi
{

class Camera
{
  public:
    Camera(const SwapChainDimensions &swapChainDimensions);
    Camera(const Camera &) = delete;
    Camera(Camera &&) = default;
    Camera &operator=(const Camera &) & = delete;
    Camera &operator=(Camera &&) & = default;
    ~Camera() = default;

    void update(float deltaTime);

    const glm::mat4 &getViewProjectionMatrix()
    {
        return viewProjectionMatrix;
    }

  private:
    void initialize();

    const SwapChainDimensions &swapChainDimensions;

    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 clipMatrix;
    glm::mat4 viewProjectionMatrix;
};

} // namespace Tobi