#include <cassert>
#include <iostream>

#include "WindowXcb.hpp"

namespace Tobi
{
    WindowXcb::WindowXcb(WindowSettings windowSettings)
        : windowSettings(windowSettings), 
        connection(nullptr),
        screen(nullptr),
        atomWmDeleteWindow(nullptr),
        window(0)
    { }

    WindowXcb::~WindowXcb() 
    {
        if(instance)
        {
            vkDestroyInstance(instance, nullptr);
        }

        xcb_destroy_window(connection, window);
        xcb_disconnect(connection);
        free(atomWmDeleteWindow);
    }

    void WindowXcb::createWindow()
    {
        assert(windowSettings.width > 0);
        assert(windowSettings.height > 0);

        initConnection();
        initWindow();

        initInstanceExtensionNames();
        initInstance("TobiApp");
    }

    void WindowXcb::initConnection()
    {
        int scr;
        connection = xcb_connect(nullptr, &scr);
        if(connection == nullptr || xcb_connection_has_error(connection))
        {
            std::cout << "Unable to make an XCB connection" << std::endl;
            exit(-1); // change to throw exception instead, and log exceptions when caught
        }

        const auto setup = xcb_get_setup(connection);
        auto setupIterator = xcb_setup_roots_iterator(setup);

        while(scr-- > 0)
        {
            xcb_screen_next(&setupIterator);
        }

        screen = setupIterator.data;
    }

    void WindowXcb::initWindow()
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
        
        atomWmDeleteWindow = xcb_intern_atom_reply(
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
                &(*atomWmDeleteWindow).atom);
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
            {
                break;
            }
        }
        free(e);
    }

    void WindowXcb::initInstanceExtensionNames()
    {
        instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef __ANDROID__
        instanceExtensionNames.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
        instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
        instanceExtensionNames.push_back(VK_KHR_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND)
        instanceExtensionNames.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#else
        instanceExtensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
    }

    void WindowXcb::initInstance(char const *const applicationShortName)
    {
        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pApplicationName = applicationShortName;
        applicationInfo.applicationVersion = 1;
        applicationInfo.pEngineName = applicationShortName;
        applicationInfo.engineVersion = 1;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayerNames.size());
        instanceCreateInfo.ppEnabledLayerNames = instanceLayerNames.size() 
            ? instanceLayerNames.data() 
            : nullptr;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames.data();

        auto result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        assert(result == VK_SUCCESS);
    }
}  // Tobi

