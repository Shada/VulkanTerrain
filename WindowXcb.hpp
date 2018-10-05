#include <cstdint>
#include <xcb/xcb.h>

namespace Tobi
{
    struct WindowSettings
    {
        uint32_t width;
        uint32_t height;
    };
    class WindowXcb
    {
        public:
            WindowXcb(WindowSettings windowSettings);
            ~WindowXcb();

            void createWindow();

            xcb_connection_t *getConnection() { return connection; }
            xcb_window_t &getWindow() { return window; }
            
            uint32_t getWidth() { return windowSettings.width; }
            uint32_t getHeight() { return windowSettings.height; };
        private:

            void init_window();
            void init_connection();
            
            WindowSettings windowSettings;

            xcb_connection_t *connection;
            xcb_screen_t *screen;
            xcb_window_t window;
            xcb_intern_atom_reply_t *atom_wm_delete_window;
    };
}  // Tobi

