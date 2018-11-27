#include "VulkanEngine/Model/VertexLayout.hpp"

namespace TobiEngine
{

VertexLayout::VertexLayout(std::vector<VertexComponent> components)
{
    this->components = std::move(components);
}

uint32_t VertexLayout::getStride() const
{
    uint32_t res = 0;
    for (auto &component : components)
    {
        switch (component)
        {
        case VERTEX_COMPONENT_UV:
            res += 2 * sizeof(float);
            break;
        case VERTEX_COMPONENT_DUMMY_FLOAT:
            res += sizeof(float);
            break;
        case VERTEX_COMPONENT_DUMMY_VEC4:
            res += 4 * sizeof(float);
            break;
        default:
            // All components except the ones listed above are made up of 3 floats
            res += 3 * sizeof(float);
        }
    }
    return res;
}

} // namespace TobiEngine
