#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "../framework/Common.hpp"
#include "../platform/SwapChainDimensions.hpp"
#include "../framework/EventDispatcher.hpp"

#include "KeyCodes.hpp"

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
        switch (event.key)
        {
        case TobiKeyCodes::TOBI_KEY_W:
        {
            if (movingDirection.z == 0)
                movingDirection.z = -1.f;
            else
                movingDirection.z = 0.f;
        }
        break;
        case TobiKeyCodes::TOBI_KEY_S:
        {
            if (movingDirection.z == 0)
                movingDirection.z = 1.f;
            else
                movingDirection.z = 0.f;
        }
        break;
        case TobiKeyCodes::TOBI_KEY_A:
        {
            if (movingDirection.x == 0)
                movingDirection.x = -1.f;
            else
                movingDirection.x = 0.f;
        }
        break;
        case TobiKeyCodes::TOBI_KEY_D:
        {
            if (movingDirection.x == 0)
                movingDirection.x = 1.f;
            else
                movingDirection.x = 0.f;
        }
        break;
        default:
        {
        }
        break;
        }
    };

    virtual void onEvent(KeyReleaseEvent &event, Dispatcher<KeyReleaseEvent> &sender)
    {
        switch (event.key)
        {
        case TobiKeyCodes::TOBI_KEY_W:
        {
            if (movingDirection.z < 0)
                movingDirection.z = 0;
        }
        break;
        case TobiKeyCodes::TOBI_KEY_S:
        {
            if (movingDirection.z > 0)
                movingDirection.z = 0;
        }
        break;
        case TobiKeyCodes::TOBI_KEY_A:
        {
            if (movingDirection.x < 0)
                movingDirection.x = 0;
        }
        break;
        case TobiKeyCodes::TOBI_KEY_D:
        {
            if (movingDirection.x > 0)
                movingDirection.x = 0;
        }
        break;
        default:
        {
        }
        break;
        }
    };

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