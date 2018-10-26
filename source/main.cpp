#include <memory>
#include <iostream>

#include "framework/Context.hpp"

// TODO: change so that platform is within context, and is not returned.
// Only use the context to interact with vulkan platform

namespace Tobi
{

int init()
{
    auto context = std::make_shared<Context>();

    context->initialize();

    return 0;
}

} // namespace Tobi

int main()
{
    Tobi::init();

    //Tobi::run();

    return 0;
}