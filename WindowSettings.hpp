#pragma once

#include <cstdint>
#include <string>

namespace Tobi
{

typedef struct TWindowSettings
{
    TWindowSettings(uint32_t width, uint32_t height, std::string applicationName)
        : width(width),
          height(height),
          applicationName(applicationName)
    {
    }
    uint32_t width;
    uint32_t height;
    std::string applicationName;
} WindowSettings;

} // namespace Tobi