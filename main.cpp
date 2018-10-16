#include <iostream>
#include <stdexcept>
#include <memory>

#include "VulkanCore.hpp"

#include "EventDispatcher.hpp"

namespace
{

void startProgram()
{
    auto resizeWindowDispatcher = std::make_shared<Tobi::ResizeWindowDispatcher>();
    auto vulkanCore = std::make_shared<Tobi::VulkanCore>(resizeWindowDispatcher);

    resizeWindowDispatcher->Reg(vulkanCore);

    vulkanCore->run();

    resizeWindowDispatcher->Unreg(vulkanCore);

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
