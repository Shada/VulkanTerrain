#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>
#include <xcb/xcb.h>

#include "WindowSettings.hpp"
#include "EventDispatcher.hpp"

namespace Tobi
{

class Game;
typedef struct TLayerProperties
{
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> instanceExtensions;
    std::vector<VkExtensionProperties> deviceExtensions;
} LayerProperties;

typedef struct TGpu
{
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    VkPhysicalDeviceProperties properties;
} Gpu;

class WindowXcb
{
  public:
    WindowXcb(std::shared_ptr<WindowSettings> windowSettings, std::shared_ptr<ResizeWindowDispatcher> resizeWindowDispatcher);
    WindowXcb(const WindowXcb &) = delete;
    WindowXcb(WindowXcb &&) = default;
    WindowXcb &operator=(const WindowXcb &) & = delete;
    WindowXcb &operator=(WindowXcb &&) & = default;
    ~WindowXcb();

    void pollEvents();
    void handleEvent(const xcb_generic_event_t *event);

    void waitForDeviceIdle();

    bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex);

    xcb_connection_t *getConnection() { return connection; }
    const xcb_window_t &getWindow() { return window; }

    const uint32_t &getWidth() { return windowSettings->width; }
    const uint32_t &getHeight() { return windowSettings->height; };

    const VkInstance &getInstance() { return instance; }

    const VkPhysicalDevice &getPhysicalDevice() { return gpus[0].physicalDevice; }

    const VkDevice &getDevice() { return device; }

    const VkSurfaceKHR &getSurface() { return surface; }
    const VkFormat &getSurfaceFormat() { return surfaceFormat; }

    const uint32_t &getGraphicsQueueIndex() { return graphicsQueueFamilyIndex; }
    const uint32_t &getPresentQueueIndex() { return presentQueueFamilyIndex; }

    const VkQueue &getGraphicsQueue() { return graphicsQueue; }
    const VkQueue &getPresentQueue() { return presentQueue; }

    const bool &isRunning() { return running; }

  private:
    void createWindow();

    void initConnection();
    void initWindow();

    void initInstanceExtensionNames();
    VkResult initDeviceExtensionProperties(LayerProperties &layerProperties);
    void initDeviceExtensionNames();

    // Add to applicationShortName "Window" settings, and rename WindowSettings to ApplicationBaseSettings or better
    void initInstance();

    VkResult initEnumerateDevice(uint32_t gpu_count = 1);

    void initSurface();
    void initDeviceQueueFamilies();

    VkResult initDevice();

    void initDeviceQueues();

    // settings
    std::shared_ptr<WindowSettings> windowSettings;

    std::unique_ptr<Game> game;

    std::shared_ptr<ResizeWindowDispatcher> resizeWindowDispatcher;

    // xcb window
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_atom_t atomWmProtocol;
    xcb_atom_t atomWmDeleteWindow;
    xcb_window_t window;

    // extensions and layers
    std::vector<const char *> instanceExtensionNames;
    std::vector<const char *> instanceLayerNames;
    std::vector<LayerProperties> instanceLayerProperties;
    std::vector<const char *> deviceExtensionNames;

    // instance
    VkInstance instance;

    // physical devices. Currently only support 1 (the first) device, but have prepared for multi gpu
    // Next should select the best device, and later support many devices.
    std::vector<Gpu> gpus;

    // logical device
    VkDevice device;

    // surface
    VkSurfaceKHR surface;
    VkFormat surfaceFormat;

    VkExtent2D extent;

    // queues
    uint32_t queueFamilyCount;
    std::vector<VkQueueFamilyProperties> queueProperties;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    bool running;
};
} // namespace Tobi
