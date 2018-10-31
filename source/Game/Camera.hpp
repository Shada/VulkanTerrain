#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "../framework/Common.hpp"
#include "../platform/SwapChainDimensions.hpp"
#include "../framework/EventDispatcher.hpp"

#include "KeyCodes.hpp"

namespace Tobi
{

class Camera
{
  public:
    Camera(const SwapChainDimensions &swapChainDimensions);
    Camera(const Camera &) = delete;
    Camera(Camera &&) = delete;
    Camera &operator=(const Camera &) & = delete;
    Camera &operator=(Camera &&) & = delete;
    ~Camera() = default;

    void update(float deltaTime);

    const glm::mat4 &getViewProjectionMatrix()
    {
        return viewProjectionMatrix;
    }

  private:
    void initialize();

    const SwapChainDimensions &swapChainDimensions;

    // free moving camera has a look direction
    glm::vec3 position;
    glm::vec3 lookDirection;
    glm::vec3 up;

    glm::vec3 movingDirection;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 clipMatrix;
    glm::mat4 viewProjectionMatrix;
};

} // namespace Tobi