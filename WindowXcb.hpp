#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <xcb/xcb.h>

#include "WindowSettings.hpp"

namespace Tobi
{
    class WindowXcb
    {
        public:
            WindowXcb(WindowSettings windowSettings);
            WindowXcb(const WindowXcb&) = delete;
            WindowXcb(WindowXcb&&) = default;
            WindowXcb& operator=(const WindowXcb&) & = delete;
            WindowXcb& operator=(WindowXcb&&) & = default;
            ~WindowXcb();

            void createWindow();

            xcb_connection_t *getConnection() { return connection; }
            const xcb_window_t &getWindow() { return window; }
            
            const uint32_t &getWidth() { return windowSettings.width; }
            const uint32_t &getHeight() { return windowSettings.height; };

            const VkInstance &getInstance() { return instance; }

        private:

            void initConnection();
            void initWindow();

            void initInstanceExtensionNames();
            // Add to applicationShortName "Window" settings, and rename WindowSettings to ApplicationBaseSettings or better
            void initInstance(char const *const applicationShortName);
            
            WindowSettings windowSettings;

            xcb_connection_t *connection;
            xcb_screen_t *screen;
            xcb_intern_atom_reply_t *atomWmDeleteWindow;
            xcb_window_t window;

            std::vector<const char *> instanceExtensionNames;
            std::vector<const char *> instanceLayerNames;

            VkInstance instance;
    };
}  // Tobi

