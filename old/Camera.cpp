#include "Camera.hpp"

namespace Tobi
{

Camera::Camera(std::shared_ptr<WindowSettings> windowSettings)
    : windowSettings(windowSettings),
      position(glm::vec3(-5, 3, -10)),
      lookAt(glm::vec3(0, 0, 0)),
      up(glm::vec3(0, -1, 0))

{
    initCamera();
}

void Camera::initCamera()
{
    float fov = glm::radians(45.0f);
    if (windowSettings->width > windowSettings->height)
    {
        fov *= static_cast<float>(windowSettings->height) / static_cast<float>(windowSettings->width);
    }
    projectionMatrix = glm::perspective(fov, static_cast<float>(windowSettings->width) / static_cast<float>(windowSettings->height), 0.1f, 100.0f);

    viewMatrix = glm::lookAt(position, // Camera is at (-5,3,-10), in World Space
                             lookAt,   // and looks at the origin
                             up        // Head is up (set to 0,-1,0 to look upside-down)
    );
    modelMatrix = glm::mat4(1.0f);

    // Vulkan clip space has inverted Y and half Z.
    clipMatrix = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);

    modelViewProjectionMatrix = clipMatrix * projectionMatrix * viewMatrix * modelMatrix;
}

} // namespace Tobi