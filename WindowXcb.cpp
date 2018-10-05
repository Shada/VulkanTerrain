#include <cassert>
#include <iostream>

#include "WindowXcb.hpp"

namespace Tobi
{
    WindowXcb::WindowXcb(WindowSettings windowSettings)
        : windowSettings(windowSettings),
        connection(nullptr),
        screen(nullptr),
        atom_wm_delete_window(nullptr)
    { }

    WindowXcb::~WindowXcb() 
    {
        xcb_destroy_window(connection, window);
        xcb_disconnect(connection);
        free(atom_wm_delete_window);
    } 

    void WindowXcb::init_connection()
    {
        int scr;
        connection = xcb_connect(NULL, &scr);
        if(connection == NULL || xcb_connection_has_error(connection))
        {
            std::cout << "Unable to make an XCB connection" << std::endl;
            exit(-1); // change to throw exception instead, and log exceptions when caught
        }

        const auto setup = xcb_get_setup(connection);
        auto setup_iterator = xcb_setup_roots_iterator(setup);

        while(scr-- > 0)
            xcb_screen_next(&setup_iterator);

        screen = setup_iterator.data;
    }

    void WindowXcb::init_window()
    {
        assert(connection != nullptr);
        
        window = xcb_generate_id(connection);

        auto valueMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;                                           
        
        uint32_t valueList[32];
        valueList[0] = screen->black_pixel;                                                          
        valueList[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

        xcb_create_window(
                connection, 
                XCB_COPY_FROM_PARENT, 
                window, 
                screen->root, 
                0, 
                0, 
                windowSettings.width, 
                windowSettings.height, 
                0,
                XCB_WINDOW_CLASS_INPUT_OUTPUT,
                screen->root_visual,
                valueMask,
                valueList);
        
        auto protocolCookie = xcb_intern_atom(
                connection, 
                1, 
                12, 
                "WM_PROTOCOLS");       
        auto reply = xcb_intern_atom_reply(
                connection,
                protocolCookie,
                0);
        auto deleteCookie = xcb_intern_atom(
                connection,
                0,
                16,
                "WM_DELETE_WINDOW");
        atom_wm_delete_window = xcb_intern_atom_reply(
                connection,
                deleteCookie,
                0);

        xcb_change_property(
                connection,
                XCB_PROP_MODE_REPLACE,
                window,
                (*reply).atom,
                4,
                32,
                1,
                &(*atom_wm_delete_window).atom);
        free(reply);

        xcb_map_window(connection, window);

        const uint32_t coords[] = {100, 100};
        xcb_configure_window(
                connection,
                window,
                XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
                coords);
        xcb_flush(connection);

        xcb_generic_event_t *e;
        while((e = xcb_wait_for_event(connection)))
        {
            if((e->response_type & ~0x80) == XCB_EXPOSE)
                break;
        }
        free(e);
    }

    void WindowXcb::createWindow()
    {
        assert(windowSettings.width > 0);
        assert(windowSettings.height > 0);

        init_connection();

        init_window();
        std::cout<<"Creating Window: " 
            << windowSettings.height 
            << "x" 
            << windowSettings.width <<std::endl;
    }
}  // Tobi

