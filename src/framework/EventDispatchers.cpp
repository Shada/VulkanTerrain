#include "EventDispatchers.hpp"

namespace Tobi
{
std::unique_ptr<KeyPressDispatcher> EventDispatchersStruct::keyPressDispatcher = std::make_unique<KeyPressDispatcher>();
std::unique_ptr<KeyReleaseDispatcher> EventDispatchersStruct::keyReleaseDispatcher = std::make_unique<KeyReleaseDispatcher>();

} // namespace Tobi