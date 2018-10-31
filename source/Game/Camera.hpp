#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "../framework/Common.hpp"

#include "../platform/SwapChainDimensions.hpp"

#include "../framework/EventDispatcher.hpp"

namespace Tobi
{

class Camera : public Dispatcher<KeyPressEvent>::Listener,
               public Dispatcher<KeyReleaseEvent>::Listener
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

    virtual void onEvent(KeyPressEvent &event, Dispatcher<KeyPressEvent> &sender)
    {
        // if press W
        movingDirection.z = -1;
    };

    virtual void onEvent(KeyReleaseEvent &event, Dispatcher<KeyReleaseEvent> &sender)
    {
        // if press W
        movingDirection.z = 0;
    };

  private:
    void initialize();

    const SwapChainDimensions &swapChainDimensions;

    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;

    glm::vec3 movingDirection;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 clipMatrix;
    glm::mat4 viewProjectionMatrix;
};

} // namespace Tobi