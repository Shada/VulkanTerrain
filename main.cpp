#include <iostream>
#include <stdexcept>
#include <memory>

#include "VulkanCore.hpp"

namespace
{

void startProgram()
{
    auto vulkanCore = std::make_unique<Tobi::VulkanCore>();

    std::cout << "this is just for the breakpoint" << std::endl;
}

} // namespace

int main()
{
    try
    {
        startProgram();
    }
    catch (std::exception e)
    {
        std::cout << "Exception was caught: " << e.what() << std::endl;
    }

    return 0;
}
