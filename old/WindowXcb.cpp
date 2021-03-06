#include <cassert>
#include <iostream>
#include <unistd.h>

#include "WindowXcb.hpp"

#include "Utils/AssertTypeDefine.hpp"
#include "Game.hpp"

namespace Tobi
{
WindowXcb::WindowXcb(std::shared_ptr<WindowSettings> windowSettings, std::shared_ptr<ResizeWindowDispatcher> resizeWindowDispatcher)
    : game(std::make_unique<Game>()),
      windowSettings(windowSettings),
      connection(nullptr),
      screen(nullptr),
      atomWmDeleteWindow(XCB_ATOM_NONE),
      atomWmProtocol(XCB_ATOM_NONE),
      window(XCB_WINDOW_NONE),
      running(true),
      resizeWindowDispatcher(resizeWindowDispatcher)
{
    createWindow();
}

WindowXcb::~WindowXcb()
{
    if (device)
    {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);
    }

    if (surface)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    if (instance)
    {
        vkDestroyInstance(instance, nullptr);
    }

    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);
}

void WindowXcb::pollEvents()
{
    xcb_generic_event_t *event;
    while ((event = xcb_poll_for_event(connection)) != nullptr)
    {
        auto code = event->response_type & ~0x80;
        switch (code)
        {
        case XCB_CLIENT_MESSAGE:
            if (reinterpret_cast<xcb_client_message_event_t *>(event)->data.data32[0] == atomWmDeleteWindow)
                running = false; // status = STATUS_TEARDOWN;
            break;

        case XCB_DESTROY_NOTIFY:
            running = false; // status = STATUS_TEARDOWN;
            break;
        }
        free(event);
    }
}

void WindowXcb::waitForDeviceIdle()
{
    vkDeviceWaitIdle(device);
}

void WindowXcb::createWindow()
{
    assert(windowSettings->width > 0);
    assert(windowSettings->height > 0);

    initConnection();
    initWindow();

    initInstanceExtensionNames();
    initDeviceExtensionNames();
    initInstance();
    initEnumerateDevice();

    initSurface();
    initDeviceQueueFamilies();
    initDevice();
    initDeviceQueues();
}

void WindowXcb::initConnection()
{
    connection = xcb_connect(nullptr, nullptr);

    if (connection == nullptr || xcb_connection_has_error(connection))
    {
        xcb_disconnect(connection);
        throw std::runtime_error("Failed to connect xcb display server");
    }

    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
}

namespace
{

xcb_intern_atom_cookie_t intern_atom_cookie(xcb_connection_t *connection, const std::string &atomName)
{
    return xcb_intern_atom(connection, false, atomName.size(), atomName.c_str());
}

xcb_atom_t intern_atom(xcb_connection_t *connection, xcb_intern_atom_cookie_t cookie)
{
    xcb_atom_t atom = XCB_ATOM_NONE;
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, nullptr);
    if (reply)
    {
        atom = reply->atom;
        free(reply);
    }

    return atom;
}

} // namespace
void WindowXcb::initWindow()
{
    assert(connection != nullptr);

    window = xcb_generate_id(connection);

    const uint32_t events[] =
        {
            XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
            XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_ENTER_WINDOW |
            XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE |
            XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
            XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_KEY_PRESS |
            XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE // wanna remove the last one eventually
        };
    xcb_create_window(
        connection,
        XCB_COPY_FROM_PARENT,
        window,
        screen->root,
        0,
        0,
        windowSettings->width,
        windowSettings->height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        XCB_CW_EVENT_MASK,
        events);

    // set WINDOW title
    xcb_change_property(
        connection,
        XCB_PROP_MODE_REPLACE,
        window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        windowSettings->applicationName.size(),
        windowSettings->applicationName.c_str());
    xcb_change_property(
        connection,
        XCB_PROP_MODE_REPLACE,
        window,
        XCB_ATOM_WM_ICON_NAME,
        XCB_ATOM_STRING,
        8,
        windowSettings->applicationName.size(),
        windowSettings->applicationName.c_str());

    auto protocolCookie = intern_atom_cookie(connection, "WM_PROTOCOLS");
    auto deleteCookie = intern_atom_cookie(connection, "WM_DELETE_WINDOW");

    atomWmProtocol = intern_atom(connection, protocolCookie);
    atomWmDeleteWindow = intern_atom(connection, deleteCookie);

    // advertise WM_DELETE_WINDOW
    xcb_change_property(
        connection,
        XCB_PROP_MODE_REPLACE,
        window,
        atomWmProtocol,
        XCB_ATOM_ATOM,
        32,
        1,
        &atomWmDeleteWindow);

    xcb_map_window(connection, window);
    xcb_aux_sync(connection);

    pollEvents();
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

VkResult WindowXcb::initDeviceExtensionProperties(LayerProperties &layerProperties)
{
    VkResult result = VK_SUCCESS;

    auto layerName = layerProperties.properties.layerName;

    for (auto &gpu : gpus)
    {
        do
        {
            uint32_t deviceExtensionCount;

            result = vkEnumerateDeviceExtensionProperties(
                gpu.physicalDevice,
                layerName,
                &deviceExtensionCount,
                nullptr);

            if (result)
            {
                break;
            }

            if (deviceExtensionCount == 0)
            {
                break;
            }

            layerProperties.deviceExtensions.resize(deviceExtensionCount);
            auto deviceExtensions = layerProperties.deviceExtensions.data();
            result = vkEnumerateDeviceExtensionProperties(
                gpu.physicalDevice,
                layerName,
                &deviceExtensionCount,
                deviceExtensions);
        } while (result == VK_INCOMPLETE);
    }

    return result;
}

void WindowXcb::initDeviceExtensionNames()
{
    deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void WindowXcb::initInstance()
{
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = windowSettings->applicationName.c_str();
    applicationInfo.applicationVersion = 1;
    applicationInfo.pEngineName = windowSettings->applicationName.c_str();
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

VkResult WindowXcb::initEnumerateDevice(uint32_t gpuCount)
{
    auto const U_ASSERT_ONLY requiredGpuCount = gpuCount;
    auto result = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    assert(gpuCount);

    gpus.resize(gpuCount);
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);

    result = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
    assert(!result && gpuCount >= requiredGpuCount);

    for (uint32_t i = 0; i < gpuCount; i++)
    {
        gpus[i].physicalDevice = physicalDevices[i];
    }

    for (auto &gpu : gpus)
    {
        vkGetPhysicalDeviceQueueFamilyProperties(gpu.physicalDevice, &queueFamilyCount, nullptr);
        assert(queueFamilyCount >= 1);

        queueProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            gpu.physicalDevice,
            &queueFamilyCount,
            queueProperties.data());
        assert(queueFamilyCount >= 1);

        vkGetPhysicalDeviceMemoryProperties(gpu.physicalDevice, &gpu.memoryProperties);
        vkGetPhysicalDeviceProperties(gpu.physicalDevice, &gpu.properties);

        for (auto &layerProperties : instanceLayerProperties)
        {
            initDeviceExtensionProperties(layerProperties);
        }
    }

    return result;
}

// move into physical device struct (in won file)? do for each device?
bool WindowXcb::memoryTypeFromProperties(
    uint32_t typeBits,
    VkFlags requirementsMask,
    uint32_t *typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < gpus[0].memoryProperties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            // Type is available, does it match user properties?
            if ((gpus[0].memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

void WindowXcb::initSurface()
{
    VkResult U_ASSERT_ONLY result = VK_SUCCESS;

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.hinstance = connection;
    surfaceCreateInfo.hwnd = window;
    result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(__ANDROID__)
    // this is not yet supported
    /*
        GET_INSTANCE_PROC_ADDR(instance, CreateAndroidSurfaceKHR);

        VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.window = AndroidGetApplicationWindow();
        result = fpCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
        */
#elif defined(VK_USE_PLATFOR_IOS_MVK)
    VkIOSSurfaceInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_IOS_CREATE_INFO_MVK;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.pView = window;
    result = vkCreateIOSSurfaceMVK(instance, &surfaceCreateInfo, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    VkMacOSSurfaceCreateInfo surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_STRUCTURE_CREATE_INFO_MVK;
    surfaceCreateInfo.pNext = nulptr;
    surfaceCreateInfo.flags - 0;
    surfaceCreateInfo.pView = window;
#elif defined(VK_USE_PLATFORM_WAYLAND)
    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.display = display;
    surfaceCreateInfo.surface = window;
    result = vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#else
    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.connection = connection;
    surfaceCreateInfo.window = window;
    result = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#endif
    assert(result == VK_SUCCESS);

    uint32_t formatCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(getPhysicalDevice(), surface, &formatCount, nullptr);
    assert(result == VK_SUCCESS);
    VkSurfaceFormatKHR *surfaceFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(getPhysicalDevice(), surface, &formatCount, surfaceFormats);
    assert(result == VK_SUCCESS);

    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
        assert(formatCount >= 1);
        surfaceFormat = surfaceFormats[0].format;
    }

    free(surfaceFormats);
}

// TODO: add transfer and compute queues
void WindowXcb::initDeviceQueueFamilies()
{
    VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(getPhysicalDevice(), i, surface, &pSupportsPresent[i]);
    }

    graphicsQueueFamilyIndex = UINT32_MAX;
    presentQueueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (graphicsQueueFamilyIndex == UINT32_MAX)
                graphicsQueueFamilyIndex = i;

            if (pSupportsPresent[i] == VK_TRUE)
            {
                graphicsQueueFamilyIndex = i;
                presentQueueFamilyIndex = i;
                break;
            }
        }
    }

    if (presentQueueFamilyIndex == UINT32_MAX)
    {
        for (size_t i = 0; i < queueFamilyCount; ++i)
        {
            if (pSupportsPresent[i] == VK_TRUE)
            {
                presentQueueFamilyIndex = i;
                break;
            }
        }
    }
    free(pSupportsPresent);

    if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX)
    {
        std::cout << "Could not find a queue for both graphics and present" << std::endl;
        exit(-1);
    }
}

VkResult WindowXcb::initDevice()
{
    VkResult result = VK_SUCCESS;

    float queuePriorities[1] = {0.0};
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = queuePriorities;
    deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceCreateInfo.enabledExtensionCount
                                                   ? deviceExtensionNames.data()
                                                   : nullptr;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    result = vkCreateDevice(getPhysicalDevice(), &deviceCreateInfo, nullptr, &device);
    assert(result == VK_SUCCESS);

    return result;
}

void WindowXcb::initDeviceQueues()
{ /* DEPENDS on init_swapchain_extension() */

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    if (graphicsQueueFamilyIndex == presentQueueFamilyIndex)
    {
        presentQueue = graphicsQueue;
    }
    else
    {
        vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
    }
}

} // namespace Tobi
