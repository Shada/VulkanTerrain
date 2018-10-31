#pragma once
#include "../Platform.hpp"

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

namespace Tobi
{

class PlatformXcb : public Platform
{
  public:
    PlatformXcb();
    PlatformXcb(const PlatformXcb &) = delete;
    PlatformXcb(PlatformXcb &&) = delete;
    PlatformXcb &operator=(const PlatformXcb &) & = delete;
    PlatformXcb &operator=(PlatformXcb &&) & = delete;
    ~PlatformXcb();

    /// @brief Presents an image to the swapchain.
    /// @param index The swapchain index previously obtained from @ref
    /// acquireNextImage.
    /// @returns Error code.
    virtual Result presentImage(uint32_t index, const VkSemaphore &releaseSemaphore) override;

    virtual const TobiStatus &getWindowStatus() const { return status; }

  private:
    Display *display;
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atom_delete_window;
    TobiStatus status;

    virtual Result initialize() override;

    virtual Result initWindow() override;

    virtual Result initSurface() override;

    void handleEvents();
};

} // namespace Tobi