/* Copyright (c) 2018 Tobi (Shada) Andersson */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "../libvulkan-loader.hpp"

/// @brief Helper macro to test the result of Vulkan calls which can return an
/// error.
#define VK_CHECK(x)                                                                     \
    do                                                                                  \
    {                                                                                   \
        VkResult err = x;                                                               \
        if (err)                                                                        \
        {                                                                               \
            LOGE("Detected Vulkan error %d at %s:%d.\n", int(err), __FILE__, __LINE__); \
            abort();                                                                    \
        }                                                                               \
    } while (0)

#define ASSERT_VK_HANDLE(handle)                                    \
    do                                                              \
    {                                                               \
        if ((handle) == VK_NULL_HANDLE)                             \
        {                                                           \
            LOGE("Handle is NULL at %s:%d.\n", __FILE__, __LINE__); \
            abort();                                                \
        }                                                           \
    } while (0)

