#pragma once
#include "../Platform.hpp"

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

namespace Tobi
{

class PlatformXcb : public Platform
{
  public:
    PlatformXcb();
    ~PlatformXcb();

  private:
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atom_delete_window;
    Platform::Status status;

    virtual Result initialize() override;

    virtual Result initWindow() override;

    void handleEvents();
};

} // namespace Tobi