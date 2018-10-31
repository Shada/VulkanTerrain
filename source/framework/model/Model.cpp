#include "Model.hpp"

#include <cstdio>
#include <string.h>

namespace Tobi
{

Model::Model(const char *filename)
    : vertices(std::vector<Vertex>()),
      filename(filename)
{
    initialize();
}

void Model::initialize()
{
    if (strcmp(filename, "triangle") == 0)
    {
        vertices = {
            {
                glm::vec4(-0.5f, -0.5f, -2.0f, 1.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, 1.0f),
            },
            {
                glm::vec4(1.5f, -0.5f, -2.0f, 1.0f),
                glm::vec4(0.0f, +0.0f, +1.0f, 1.0f),
            },
            {
                glm::vec4(-0.5f, 0.5f, -2.0f, 1.0f),
                glm::vec4(+0.0f, 1.0f, +0.0f, 1.0f),
            }};
    }
    else if (strcmp(filename, "cube") == 0)
    {
        vertices = {
            // red face
            {
                glm::vec4(-1.f, -1.f, 1.f, 1.f),
                glm::vec4(+1.f, +0.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, -1.f, 1.f, 1.f),
                glm::vec4(1.f, +0.f, 0.f, 1.f),
            },
            {
                glm::vec4(-1.f, 1.f, 1.f, 1.f),
                glm::vec4(+1.f, 0.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, -1.f, 1.f, 1.f),
                glm::vec4(1.f, +0.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, 1.f, 1.f, 1.f),
                glm::vec4(1.f, 0.f, 0.f, 1.f),
            },
            {
                glm::vec4(-1.f, 1.f, 1.f, 1.f),
                glm::vec4(+1.f, 0.f, 0.f, 1.f),
            },
            // green face
            {
                glm::vec4(-1.f, -1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(-1.f, +1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(+1.f, -1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(-1.f, +1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(+1.f, +1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(+1.f, -1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 0.f, 1.f),
            },
            // blue face
            {
                glm::vec4(-1.f, +1.f, +1.f, 1.f),
                glm::vec4(0.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, +1.f, -1.f, 1.f),
                glm::vec4(0.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, -1.f, +1.f, 1.f),
                glm::vec4(0.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, +1.f, -1.f, 1.f),
                glm::vec4(0.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, -1.f, -1.f, 1.f),
                glm::vec4(0.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, -1.f, +1.f, 1.f),
                glm::vec4(0.f, 0.f, 1.f, 1.f),
            },
            // yellow face
            {
                glm::vec4(1.f, +1.f, +1.f, 1.f),
                glm::vec4(1.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, -1.f, +1.f, 1.f),
                glm::vec4(1.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, +1.f, -1.f, 1.f),
                glm::vec4(1.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, -1.f, +1.f, 1.f),
                glm::vec4(1.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, -1.f, -1.f, 1.f),
                glm::vec4(1.f, 1.f, 0.f, 1.f),
            },
            {
                glm::vec4(1.f, +1.f, -1.f, 1.f),
                glm::vec4(1.f, 1.f, 0.f, 1.f),
            },
            // magenta face
            {
                glm::vec4(+1.f, 1.f, +1.f, 1.f),
                glm::vec4(1.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(+1.f, 1.f, -1.f, 1.f),
                glm::vec4(1.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, 1.f, +1.f, 1.f),
                glm::vec4(1.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(+1.f, 1.f, -1.f, 1.f),
                glm::vec4(1.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, 1.f, -1.f, 1.f),
                glm::vec4(1.f, 0.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, 1.f, +1.f, 1.f),
                glm::vec4(1.f, 0.f, 1.f, 1.f),
            },
            // cyan face
            {
                glm::vec4(+1.f, -1.f, +1.f, 1.f),
                glm::vec4(0.f, 1.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, -1.f, +1.f, 1.f),
                glm::vec4(0.f, 1.f, 1.f, 1.f),
            },
            {
                glm::vec4(+1.f, -1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, -1.f, +1.f, 1.f),
                glm::vec4(0.f, 1.f, 1.f, 1.f),
            },
            {
                glm::vec4(-1.f, -1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 1.f, 1.f),
            },
            {
                glm::vec4(+1.f, -1.f, -1.f, 1.f),
                glm::vec4(0.f, 1.f, 1.f, 1.f),
            }};
    }
}

} // namespace Tobi