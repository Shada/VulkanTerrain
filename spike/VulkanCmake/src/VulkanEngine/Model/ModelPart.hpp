#pragma once

#include <cstdint>

namespace TobiEngine
{

/** @brief Stores vertex and index base and counts for each part of a model */
struct ModelPart
{
    ModelPart() : vertexBase(0), vertexCount(0), indexBase(0), indexCount(0) {}
    uint32_t vertexBase;
    uint32_t vertexCount;
    uint32_t indexBase;
    uint32_t indexCount;
};

} // namespace TobiEngine