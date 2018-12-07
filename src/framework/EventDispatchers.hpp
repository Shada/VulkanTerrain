#pragma once

#include <memory>

#include "EventDispatcher.hpp"

namespace Tobi
{

struct EventDispatchersStruct
{
    static std::unique_ptr<KeyPressDispatcher> keyPressDispatcher;
    static std::unique_ptr<KeyReleaseDispatcher> keyReleaseDispatcher;
};

} // namespace Tobi
