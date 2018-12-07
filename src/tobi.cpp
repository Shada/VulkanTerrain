#include "tobi.hpp"

#include <iostream>

namespace tobi
{

Tobi::Tobi()
{
    position = glm::vec3(3.f);
    VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    std::cout << "Tobi()" << std::endl;
}

glm::vec3 Tobi::getPosition() 
{ 
    return position; 
}

Tobi createTobi()
{
    std::cout << "Let's create a Tobi!" << std::endl;
    return Tobi();   
}

}
