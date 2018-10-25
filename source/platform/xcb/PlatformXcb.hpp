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

    /// @brief Gets current window status.
    /// @returns Window status.
    virtual Status getWindowStatus() override;

    /// @brief Presents an image to the swapchain.
    /// @param index The swapchain index previously obtained from @ref
    /// acquireNextImage.
    /// @returns Error code.
    virtual Result presentImage(uint32_t index) override;

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