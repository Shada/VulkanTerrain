/* Copyright (c) 2018Tobi (Shada) Andersson */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifdef ANDROID
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Tobi", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Tobi", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARNING, "Tobi", __VA_ARGS__)
#else
#define LOGE(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
#define LOGI(...) fprintf(stderr, "INFO: " __VA_ARGS__)
#define LOGW(...) fprintf(stderr, "WARNING: " __VA_ARGS__)
#endif

namespace Tobi
{

/// @brief Generic error codes used throughout the framework and platform.
enum Result
{
    /// Success
    RESULT_SUCCESS = 0,

    /// Generic error without any particular information
    RESULT_ERROR_GENERIC = -1,

    /// Returned by the swapchain when the swapchain is invalid and should be
    /// recreated
    RESULT_ERROR_OUTDATED_SWAPCHAIN = -2,

    /// Generic input/output errors
    RESULT_ERROR_IO = -3,

    /// Memory allocation errors
    RESULT_ERROR_OUT_OF_MEMORY = -4
};

/// @brief Helper macro to determine success of a call.
#define SUCCEEDED(x) ((x) == RESULT_SUCCESS)
/// @brief Helper macro to determine failure of a call.
#define FAILED(x) ((x) != RESULT_SUCCESS)

} // namespace Tobi
