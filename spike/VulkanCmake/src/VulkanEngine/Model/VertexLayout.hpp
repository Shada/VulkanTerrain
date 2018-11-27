#pragma once

#include <cstdint>
#include <vector>

#include "VulkanEngine/Model/VertexComponent.hpp"

namespace TobiEngine
{

/** @brief Stores vertex layout components for model loading and Vulkan vertex input and atribute bindings  */
class VertexLayout
{
  public:
    VertexLayout(std::vector<VertexComponent> components);
    ~VertexLayout() = default;

    uint32_t getStride() const;

    const auto &getComponents() const { return components; }

  private:
    /** @brief Components used to generate vertices from */
    std::vector<VertexComponent> components;
};

} // namespace TobiEngine