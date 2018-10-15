#pragma once

#include <cstdint>
#include <string>

namespace Tobi
{

typedef struct TWindowSettings
{
    uint32_t width;
    uint32_t height;
    std::string applicationName;
} WindowSettings;

} // namespace Tobi