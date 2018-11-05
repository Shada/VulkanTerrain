#include "Model.hpp"

#include <cstdio>
#include <string.h>

#include "Meshes.teapot.hpp"

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
                glm::vec4(-0.5f, -0.5f, -2.0f, +1.0f), // position
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f), // normal
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f), // colour
            },
            {
                glm::vec4(+1.5f, -0.5f, -2.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-0.5f, +0.5f, -2.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
            }};
    }
    else if (strcmp(filename, "cube") == 0)
    {
        vertices = {
            // red face
            {
                glm::vec4(-1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
            },
            // green face
            {
                glm::vec4(-1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, -1.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, -1.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, -1.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, -1.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, -1.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +0.0f, -1.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
            },
            // blue face
            {
                glm::vec4(-1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(-1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(-1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(-1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(-1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(-1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(-1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +0.0f, +1.0f, +0.0f),
            },
            // yellow face
            {
                glm::vec4(+1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +1.0f, +0.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+1.0f, +0.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +1.0f, +0.0f, +0.0f),
            },
            // magenta face
            {
                glm::vec4(+1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, +1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, +1.0f, +0.0f, +0.0f),
                glm::vec4(+1.0f, +0.0f, +1.0f, +0.0f),
            },
            // cyan face
            {
                glm::vec4(+1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, -1.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, -1.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, -1.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, -1.0f, +1.0f, +1.0f),
                glm::vec4(+0.0f, -1.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(-1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, -1.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +1.0f, +0.0f),
            },
            {
                glm::vec4(+1.0f, -1.0f, -1.0f, +1.0f),
                glm::vec4(+0.0f, -1.0f, +0.0f, +0.0f),
                glm::vec4(+0.0f, +1.0f, +1.0f, +0.0f),
            }};
    }
    else if (strcmp(filename, "teapot") == 0)
    {
        for (auto index : teapot_indices)
        {
            auto i = index * 3;
            Vertex v;
            v.position = glm::vec4(teapot_positions[i], teapot_positions[i + 1], teapot_positions[i + 2], 1.f);
            v.normal = glm::vec4(teapot_normals[i], teapot_normals[i + 1], teapot_normals[i + 2], 0.f);
            v.colour = glm::vec4(1.f, 1.f, 1.f, 0.f);
            vertices.push_back(v);
        }
    }
}

} // namespace Tobi