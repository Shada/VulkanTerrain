#include "Model.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/cimport.h>

#include "framework/Common.hpp"

namespace Tobi
{

Model::Model(const char *filename)
    : vertices(std::vector<Vertex>()),
      filename(filename)
{
    initialize();
}

static const std::vector<Vertex> triangleMesh = {
    {
        glm::vec3(-0.5f, -0.5f, -2.0f), // position
        glm::vec3(+0.0f, +0.0f, +1.0f), // normal
        glm::vec3(+1.0f, +0.0f, +0.0f), // colour
    },
    {
        glm::vec3(+1.5f, -0.5f, -2.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, -2.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
    }};

static const std::vector<uint32_t> triangleIndices = {0, 1, 2};

static const std::vector<Vertex> cubeMesh = {
    // red face
    {
        glm::vec3(-1.0f, -1.0f, +1.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
    },
    {
        glm::vec3(+1.0f, -1.0f, +1.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
    },
    {
        glm::vec3(-1.0f, +1.0f, +1.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
    },
    {
        glm::vec3(+1.0f, +1.0f, +1.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
    },
    // green face
    {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(+0.0f, +0.0f, -1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
    },
    {
        glm::vec3(-1.0f, +1.0f, -1.0f),
        glm::vec3(+0.0f, +0.0f, -1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
    },
    {
        glm::vec3(+1.0f, -1.0f, -1.0f),
        glm::vec3(+0.0f, +0.0f, -1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
    },
    {
        glm::vec3(+1.0f, +1.0f, -1.0f),
        glm::vec3(+0.0f, +0.0f, -1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
    },
    // blue face
    {
        glm::vec3(-1.0f, +1.0f, +1.0f),
        glm::vec3(-1.0f, +0.0f, +0.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
    },
    {
        glm::vec3(-1.0f, +1.0f, -1.0f),
        glm::vec3(-1.0f, +0.0f, +0.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
    },
    {
        glm::vec3(-1.0f, -1.0f, +1.0f),
        glm::vec3(-1.0f, +0.0f, +0.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
    },
    {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, +0.0f, +0.0f),
        glm::vec3(+0.0f, +0.0f, +1.0f),
    },
    // yellow face
    {
        glm::vec3(+1.0f, +1.0f, +1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
        glm::vec3(+1.0f, +1.0f, +0.0f),
    },
    {
        glm::vec3(+1.0f, -1.0f, +1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
        glm::vec3(+1.0f, +1.0f, +0.0f),
    },
    {
        glm::vec3(+1.0f, +1.0f, -1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
        glm::vec3(+1.0f, +1.0f, +0.0f),
    },
    {
        glm::vec3(+1.0f, -1.0f, -1.0f),
        glm::vec3(+1.0f, +0.0f, +0.0f),
        glm::vec3(+1.0f, +1.0f, +0.0f),
    },
    // magenta face
    {
        glm::vec3(+1.0f, +1.0f, +1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
        glm::vec3(+1.0f, +0.0f, +1.0f),
    },
    {
        glm::vec3(+1.0f, +1.0f, -1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
        glm::vec3(+1.0f, +0.0f, +1.0f),
    },
    {
        glm::vec3(-1.0f, +1.0f, +1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
        glm::vec3(+1.0f, +0.0f, +1.0f),
    },
    {
        glm::vec3(-1.0f, +1.0f, -1.0f),
        glm::vec3(+0.0f, +1.0f, +0.0f),
        glm::vec3(+1.0f, +0.0f, +1.0f),
    },
    // cyan face
    {
        glm::vec3(+1.0f, -1.0f, +1.0f),
        glm::vec3(+0.0f, -1.0f, +0.0f),
        glm::vec3(+0.0f, +1.0f, +1.0f),
    },
    {
        glm::vec3(-1.0f, -1.0f, +1.0f),
        glm::vec3(+0.0f, -1.0f, +0.0f),
        glm::vec3(+0.0f, +1.0f, +1.0f),
    },
    {
        glm::vec3(+1.0f, -1.0f, -1.0f),
        glm::vec3(+0.0f, -1.0f, +0.0f),
        glm::vec3(+0.0f, +1.0f, +1.0f),
    },
    {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(+0.0f, -1.0f, +0.0f),
        glm::vec3(+0.0f, +1.0f, +1.0f),
    }};

static const std::vector<uint32_t> cubeIndices = {
    0, 1, 2,
    1, 3, 2,
    4, 5, 6,
    5, 7, 6,
    8, 9, 10,
    9, 11, 10,
    12, 13, 14,
    13, 15, 14,
    16, 17, 18,
    17, 19, 18,
    20, 21, 22,
    21, 23, 22};

void Model::initialize()
{
    if (strcmp(filename, "triangle") == 0)
    {
        vertices = triangleMesh;
        indices = triangleIndices;
    }
    else if (strcmp(filename, "cube") == 0)
    {
        vertices = cubeMesh;
        indices = cubeIndices;
    }
    else
    {
        Assimp::Importer importer;

        std::ifstream fin(filename);
        if (!fin.fail())
        {
            fin.close();
        }
        else
        {
            LOGE("Couldn't open file: %s \n", filename);
            LOGE("%s\n", importer.GetErrorString());
            vertices = triangleMesh;
            return;
        }

        static const int assimpFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices;

        auto scene = importer.ReadFile(filename, assimpFlags);

        if (!scene)
        {
            LOGE("%s\n", importer.GetErrorString());
            vertices = triangleMesh;
            return;
        }

        if (!scene->HasMeshes())
        {
            LOGE("%s\n", importer.GetErrorString());
            vertices = triangleMesh;
            return;
        }

        for (uint32_t m = 0; m < scene->mNumMeshes; m++)
        {
            const auto &mesh = scene->mMeshes[m];
            for (uint32_t v = 0; v < mesh->mNumVertices; v++)
            {
                Vertex vertex;

                vertex.position = glm::make_vec3(&mesh->mVertices[v].x);

                vertex.normal = glm::make_vec3(&mesh->mNormals[v].x);

                vertex.colour = (mesh->HasVertexColors(0))
                                    ? glm::make_vec3(&mesh->mColors[0][v].r)
                                    : glm::vec3(1.0f);
                vertex.position.y *= -1.f;
                vertices.push_back(vertex);
            }
        }

        for (uint32_t m = 0; m < scene->mNumMeshes; m++)
        {
            uint32_t indexBase = static_cast<uint32_t>(indices.size());
            for (uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; f++)
            {
                // We assume that all faces are triangulated
                for (uint32_t i = 0; i < 3; i++)
                {
                    indices.push_back(scene->mMeshes[m]->mFaces[f].mIndices[i] + indexBase);
                }
            }
        }
    }
}

} // namespace Tobi
