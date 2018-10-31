#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <map>

namespace Tobi
{

class ObjectManager
{
  public:
    ObjectManager();
    ObjectManager(const ObjectManager &) = delete;
    ObjectManager(ObjectManager &&) = delete;
    ObjectManager &operator=(const ObjectManager &) & = delete;
    ObjectManager &operator=(ObjectManager &&) & = delete;
    ~ObjectManager() = default;

    uint32_t addObject(uint32_t meshIndex);
    uint32_t addObject(uint32_t meshIndex, glm::vec3 position);
    uint32_t addObject(uint32_t meshIndex, glm::vec3 position, glm::vec3 rotation);
    uint32_t addObject(uint32_t meshIndex, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    const auto &getModelMatrix(uint32_t index) { return modelMatrix[index]; }
    const auto &getMeshIndex(uint32_t index) { return meshIndex[index]; }

  private:
    std::map<uint32_t, glm::vec3> position;
    std::map<uint32_t, glm::vec3> rotation;
    std::map<uint32_t, glm::vec3> scale;
    std::map<uint32_t, glm::mat4> modelMatrix;
    std::map<uint32_t, uint32_t> meshIndex;

    uint32_t idCounter;

    glm::mat4 calculateMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
};

} // namespace Tobi