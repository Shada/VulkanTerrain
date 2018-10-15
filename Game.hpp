#pragma once

#include <iostream>

namespace Tobi
{

class Game
{
  public:
    enum Key
    {
        // virtual keys
        KEY_SHUTDOWN,
        // physical keys
        KEY_UNKNOWN,
        KEY_ESC,
        KEY_UP,
        KEY_DOWN,
        KEY_SPACE,
        KEY_F,
    };

    void onKey(Key key)
    {
        if (key == Key::KEY_SHUTDOWN)
        {
            std::cout << "I wanna shoutdown now" << std::endl;
        }
    }

    void resizeSwapChain(uint32_t width, uint32_t height)
    {
        std::cout << "Let's change the window size here" << std::endl;
    }
};

} // namespace Tobi