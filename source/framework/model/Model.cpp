#include "Model.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Meshes.teapot.hpp"
#include "Common.hpp"

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

static const std::vector<Vertex> cubeMesh = {
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

void Model::initialize()
{
    if (strcmp(filename, "triangle") == 0)
    {
        vertices = triangleMesh;
    }
    else if (strcmp(filename, "cube") == 0)
    {
        vertices = cubeMesh;
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

        auto scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality);

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

        for (uint32_t meshId = 0; meshId < scene->mNumMeshes; meshId++)
        {
            const auto &mesh = scene->mMeshes[meshId];
            for (uint32_t faceId = 0; faceId < mesh->mNumFaces; faceId++)
            {
                const auto &face = mesh->mFaces[faceId];
                if (face.mNumIndices != 3)
                    continue;
                for (uint32_t i = 0; i < 3; i++)
                {
                    uint32_t vIndex = face.mIndices[i];
                    Vertex v;

                    v.position = glm::vec4(
                        mesh->mVertices[vIndex].x,
                        mesh->mVertices[vIndex].y,
                        mesh->mVertices[vIndex].z,
                        1.0f);

                    v.normal = glm::vec4(
                        mesh->mNormals[vIndex].x,
                        mesh->mNormals[vIndex].y,
                        mesh->mNormals[vIndex].z,
                        0.0f);
                    if (mesh->HasVertexColors(vIndex))
                    {
                        v.colour = glm::vec4(
                            mesh->mColors[vIndex]->r,
                            mesh->mColors[vIndex]->g,
                            mesh->mColors[vIndex]->b,
                            1.0f);
                    }
                    else
                    {
                        v.colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    }

                    vertices.push_back(v);
                }
            }
        }
    }
}

} // namespace Tobi