#include "catch2/catch.hpp"

#include "VulkanEngine/Model/VertexLayout.hpp"

TEST_CASE("Position component have stride of three floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_POSITION,
    });
    REQUIRE(layout.getStride() == (3 * sizeof(float)));
}
TEST_CASE("Color component have stride of three floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_COLOR,
    });
    REQUIRE(layout.getStride() == (3 * sizeof(float)));
}
TEST_CASE("Normal component have stride of three floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_NORMAL,
    });
    REQUIRE(layout.getStride() == (3 * sizeof(float)));
}
TEST_CASE("UV component have stride of two floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_UV,
    });
    REQUIRE(layout.getStride() == (2 * sizeof(float)));
}
TEST_CASE("Bitangent component have stride of three floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_BITANGENT,
    });
    REQUIRE(layout.getStride() == (3 * sizeof(float)));
}
TEST_CASE("Tangent component have stride of three floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_TANGENT,
    });
    REQUIRE(layout.getStride() == (3 * sizeof(float)));
}
TEST_CASE("Vec4 Dummy component have stride of four floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_DUMMY_VEC4,
    });
    REQUIRE(layout.getStride() == (4 * sizeof(float)));
}
TEST_CASE("Dummy component have stride of one float", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_DUMMY_FLOAT,
    });
    REQUIRE(layout.getStride() == (sizeof(float)));
}
TEST_CASE("No components have stride of zero floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({});
    REQUIRE(layout.getStride() == 0);
}
TEST_CASE("All components have stride of twentytwo floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_DUMMY_FLOAT,
        TobiEngine::VERTEX_COMPONENT_DUMMY_VEC4,
        TobiEngine::VERTEX_COMPONENT_BITANGENT,
        TobiEngine::VERTEX_COMPONENT_TANGENT,
        TobiEngine::VERTEX_COMPONENT_POSITION,
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
    });
    REQUIRE(layout.getStride() == (22 * sizeof(float)));
}
TEST_CASE("Regular model (position, normal, uv, color) components have stride of eleven floats", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_POSITION,
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));
}
TEST_CASE("Order of components make no difference in stride size", "[getStride]")
{
    TobiEngine::VertexLayout layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_POSITION,
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));

    layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_POSITION,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));

    layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_POSITION,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));

    layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
        TobiEngine::VERTEX_COMPONENT_POSITION,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));

    layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
        TobiEngine::VERTEX_COMPONENT_POSITION,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));

    layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_NORMAL,
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_POSITION,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));

    layout = TobiEngine::VertexLayout({
        TobiEngine::VERTEX_COMPONENT_NORMAL,
        TobiEngine::VERTEX_COMPONENT_UV,
        TobiEngine::VERTEX_COMPONENT_COLOR,
        TobiEngine::VERTEX_COMPONENT_POSITION,
    });
    REQUIRE(layout.getStride() == (11 * sizeof(float)));
}