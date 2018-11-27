#pragma once

#include <cstdint>

namespace TobiEngine
{

/** @brief Stores vertex and index base and counts for each part of a model */
struct ModelPart
{
    uint32_t vertexBase;
    uint32_t vertexCount;
    uint32_t indexBase;
    uint32_t indexCount;
};

} // namespace TobiEngine