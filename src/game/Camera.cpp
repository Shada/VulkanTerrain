#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "KeyState.hpp"

namespace Tobi
{

Camera::Camera(const SwapChainDimensions &swapChainDimensions)
    : swapChainDimensions(swapChainDimensions),
      position(glm::vec3(0.f, 0.f, 10.f)),
      lookDirection(glm::vec3(0.f, 0.f, -1.f)),
      up(glm::vec3(0.f, 1.f, 0.f))

{
    LOGI("CONSTRUCTING Camera\n");
    initialize();
}

void Camera::initialize()
{
    auto lookAt = position + lookDirection;
    viewMatrix = glm::lookAt(position, lookAt, up);

    float aspect = static_cast<float>(swapChainDimensions.width) / static_cast<float>(swapChainDimensions.height);
    projectionMatrix = glm::perspective(0.4f, aspect, 0.1f, 100.0f);

    // Vulkan clip space has inverted Y and half Z.
    clipMatrix = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

    viewProjectionMatrix = clipMatrix * projectionMatrix * viewMatrix;
}

void Camera::update(float time)
{
    if (KeyStates::keyStates[TobiKeyCodes::TOBI_KEY_W] != KeyStates::keyStates[TobiKeyCodes::TOBI_KEY_S])
    {
        if (KeyStates::keyStates[TobiKeyCodes::TOBI_KEY_S])
        {
            movingDirection.z = 1.f;
        }
        else
        {
            movingDirection.z = -1.f;
        }
    }
    else
    {
        movingDirection.z = 0.0f;
    }

    if (KeyStates::keyStates[TobiKeyCodes::TOBI_KEY_A] != KeyStates::keyStates[TobiKeyCodes::TOBI_KEY_D])
    {
        if (KeyStates::keyStates[TobiKeyCodes::TOBI_KEY_D])
        {
            movingDirection.x = 1.f;
        }
        else
        {
            movingDirection.x = -1.f;
        }
    }
    else
    {
        movingDirection.x = 0.0f;
    }

    auto speed = 0.5f;
    position += movingDirection * speed * time;

    auto lookAt = position + lookDirection;
    viewMatrix = glm::lookAt(position, lookAt, up);

    viewProjectionMatrix = clipMatrix * projectionMatrix * viewMatrix;
}

} // namespace Tobi