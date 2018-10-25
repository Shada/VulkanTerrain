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

    /// @brief Terminates the platform.
    virtual void terminate() override;

    /// @brief Gets the preferred swapchain size.
    /// @returns Error code.
    virtual SwapChainDimensions getPreferredSwapChain() override;

  protected:
  private:
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atom_delete_window;
    Platform::Status status;

    virtual Result initWindow() override;
    virtual VkSurfaceKHR createSurface() override;

    void handleEvents();
};

} // namespace Tobi