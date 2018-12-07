#include "framework/IContext.hpp"

#include "Context.hpp"

namespace Tobi
{

std::unique_ptr<IContext> IContext::create()
{
    return std::make_unique<Context>();
}

} // namespace Tobi
