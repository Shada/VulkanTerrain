#include "Camera.hpp"

namespace Tobi
{

Camera::Camera(std::shared_ptr<WindowXcb> window)
    : window(window)
{
    initCamera();
}

void Camera::initCamera()
{
    float fov = glm::radians(45.0f);
    if (window->getWidth() > window->getHeight())
    {
        fov *= static_cast<float>(window->getHeight()) / static_cast<float>(window->getWidth());
    }
    projectionMatrix = glm::perspective(fov, static_cast<float>(window->getWidth()) / static_cast<float>(window->getHeight()), 0.1f, 100.0f);

    viewMatrix = glm::lookAt(glm::vec3(-5, 3, -10), // Camera is at (-5,3,-10), in World Space
                             glm::vec3(0, 0, 0),    // and looks at the origin
                             glm::vec3(0, -1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
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