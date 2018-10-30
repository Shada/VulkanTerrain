#include "Model.hpp"

namespace Tobi
{

Model::Model()
    : vertices(std::vector<Vertex>())
{
    initialize();
}

Model::~Model()
{
}

void Model::initialize()
{
    vertices = {
        {
            glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
            glm::vec4(+1.0f, +0.0f, 0.0f, 1.0f),
        },
        {
            glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
            glm::vec4(0.0f, +0.0f, 1.0f, 1.0f),
        },
        {
            glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f),
            glm::vec4(+0.0f, 1.0f, 0.0f, 1.0f),
        }};
}

} // namespace Tobi