#include "catch2/catch.hpp"

#include "VulkanEngine/Model/Model.hpp"

#include <memory>

namespace TobiEngine
{

// TODO: the model will be in a model manager, and the model will not be added to a list, if the model does not exist
TEST_CASE("Loading a model that does not exists, return false and leaves the model empty", "[loadFromFile]")
{
    auto model = std::make_unique<Model>("notafile", VertexLayout({VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_NORMAL, VERTEX_COMPONENT_COLOR}), ModelCreateInfo());

    CHECK(model->getIndexCount() == 0);
    CHECK(model->getVertexCount() == 0);
}
TEST_CASE("Correctly loads a model without material", "[loadFromFile]")
{
    auto model = std::make_unique<Model>("assets/models/cube.obj", VertexLayout({VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_NORMAL, VERTEX_COMPONENT_COLOR}), ModelCreateInfo());

    CHECK(model->getVertexCount() == 24);
    CHECK(model->getIndexCount() == 36);
}

} // namespace TobiEngine