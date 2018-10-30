#include "ObjectManager.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Tobi
{

ObjectManager::ObjectManager()
    : position(std::map<uint32_t, glm::vec3>()),
      rotation(std::map<uint32_t, glm::vec3>()),
      scale(std::map<uint32_t, glm::vec3>()),
      modelMatrix(std::map<uint32_t, glm::mat4>()),
      meshIndex(std::map<uint32_t, uint32_t>()),
      idCounter(1)
{
}

uint32_t ObjectManager::addObject(uint32_t meshIndex, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    this->position[idCounter] = position;
    this->rotation[idCounter] = rotation;
    this->scale[idCounter] = scale;
    this->modelMatrix[idCounter] = calculateMatrix(position, rotation, scale);
    this->meshIndex[idCounter] = meshIndex;

    return idCounter++;
}

uint32_t ObjectManager::addObject(uint32_t meshIndex, glm::vec3 position, glm::vec3 rotation)
{
    return addObject(meshIndex, position, rotation, glm::vec3(0.f));
}

uint32_t ObjectManager::addObject(uint32_t meshIndex, glm::vec3 position)
{
    return addObject(meshIndex, position, glm::vec3(0.f), glm::vec3(0.f));
}

uint32_t ObjectManager::addObject(uint32_t meshIndex)
{
    return addObject(meshIndex, glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f));
}

glm::mat4 ObjectManager::calculateMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    auto identity = glm::mat4();
    auto scaleMatrix = glm::scale(identity, scale);
    auto yawMatrix = glm::rotate(identity, rotation.x, glm::vec3(0, 1, 0));
    auto pitchMatrix = glm::rotate(identity, rotation.y, glm::vec3(1, 0, 0));
    auto rollMatrix = glm::rotate(identity, rotation.z, glm::vec3(0, 0, 1));
    auto translationMatrix = glm::translate(identity, position);

    return translationMatrix * yawMatrix * pitchMatrix * rollMatrix * scaleMatrix;
}

} // namespace Tobi