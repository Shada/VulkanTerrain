#include "platform/xcb/PlatformXcb.hpp"

namespace Tobi
{

PlatformXcb::PlatformXcb()
    : connection(nullptr),
      window(0),
      atom_delete_window(nullptr)
{
    initWindow();

    initVulkan(getPreferredSwapChain(), {"VK_KHR_surface", "VK_KHR_xcb_surface"}, {"VK_KHR_swapchain"});
}

PlatformXcb::~PlatformXcb()
{
    terminate();
}

void PlatformXcb::terminate()
{
    if (connection)
    {
        xcb_aux_sync(connection);
        handleEvents();

        Platform::terminate();

        xcb_destroy_window(connection, window);
        xcb_disconnect(connection);
        free(atom_delete_window);
        connection = nullptr;
    }
}

void PlatformXcb::handleEvents()
{
    xcb_generic_event_t *event;
    while ((event = xcb_poll_for_event(connection)) != nullptr)
    {
        auto code = event->response_type & ~0x80;
        switch (code)
        {
        case XCB_CLIENT_MESSAGE:
            if (reinterpret_cast<xcb_client_message_event_t *>(event)->data.data32[0] == atom_delete_window->atom)
                status = STATUS_TEARDOWN;
            break;

        case XCB_DESTROY_NOTIFY:
            status = STATUS_TEARDOWN;
            break;
        }
        free(event);
    }
}

Result PlatformXcb::initWindow()
{
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection))
        return RESULT_ERROR_IO;

    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    window = xcb_generate_id(connection);
    const uint32_t events[] = {XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                               XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_ENTER_WINDOW |
                               XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE |
                               XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                               XCB_EVENT_MASK_FOCUS_CHANGE};

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, 1280, 720, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, XCB_CW_EVENT_MASK, events);

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, 8, "TobiTest");

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8, 8, "TobiTest");

    auto cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    auto *reply = xcb_intern_atom_reply(connection, cookie, 0);

    cookie = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    atom_delete_window = xcb_intern_atom_reply(connection, cookie, 0);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, reply->atom, 4, 32, 1, &atom_delete_window->atom);

    free(reply);

    xcb_map_window(connection, window);
    xcb_aux_sync(connection);
    handleEvents();

    status = STATUS_RUNNING;

    return RESULT_SUCCESS;
}

VkSurfaceKHR PlatformXcb::createSurface()
{
    VkSurfaceKHR surface;
    PFN_vkCreateXcbSurfaceKHR fpCreateXcbSurfaceKHR;
    if (!VULKAN_SYMBOL_WRAPPER_LOAD_INSTANCE_SYMBOL(instance, "vkCreateXcbSurfaceKHR", fpCreateXcbSurfaceKHR))
        return VK_NULL_HANDLE;

    VkXcbSurfaceCreateInfoKHR info = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    info.connection = connection;
    info.window = window;

    VK_CHECK(fpCreateXcbSurfaceKHR(instance, &info, nullptr, &surface));
    return surface;
}

Platform::SwapChainDimensions PlatformXcb::getPreferredSwapChain()
{
    SwapChainDimensions swapChainDimensions = {1280, 720, VK_FORMAT_B8G8R8A8_UNORM};
    return swapChainDimensions;
}

Platform::Status PlatformXcb::getWindowStatus()
{
    return status;
}

Result PlatformXcb::presentImage(uint32_t index)
{
    handleEvents();

    if (status == STATUS_RUNNING)
    {
        Result result = Platform::presentImage(index);
        xcb_flush(connection);
        return result;
    }
    else
        return RESULT_SUCCESS;
}

} // namespace Tobi