#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace tobi
{

class Tobi
{
public: 
    Tobi();
    ~Tobi(){};

    glm::vec3 getPosition();

private:
    glm::vec3 position;
    VkInstance instance;
};

Tobi createTobi();
}
