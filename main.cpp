#include <memory>
#include "PlatformXcb.hpp"

namespace Tobi
{

int run()
{
    std::shared_ptr<Platform> a = std::make_shared<PlatformXcb>();
    return 1;
}

} // namespace Tobi

int main()
{
    return Tobi::run();
}