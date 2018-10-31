#pragma once

#include <map>

#include "KeyCodes.hpp"
#include "../framework/EventDispatcher.hpp"

namespace Tobi
{

class KeyStates : public Dispatcher<KeyPressEvent>::Listener,
                  public Dispatcher<KeyReleaseEvent>::Listener
{
  public:
    static std::map<uint32_t, bool> keyStates;
    KeyStates() = default;

    virtual void onEvent(KeyPressEvent &event, Dispatcher<KeyPressEvent> &sender)
    {
        keyStates[event.key] = true;
    }
    virtual void onEvent(KeyReleaseEvent &event, Dispatcher<KeyReleaseEvent> &sender)
    {
        keyStates[event.key] = false;
    }
};

} // namespace Tobi