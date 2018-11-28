#include "Model.hpp"

#include <stdexcept>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace TobiEngine
{

const int Model::defaultFlags =
    aiProcess_FlipWindingOrder |
    aiProcess_Triangulate |
    aiProcess_PreTransformVertices |
    aiProcess_CalcTangentSpace |
    aiProcess_GenSmoothNormals |
    aiProcessPreset_TargetRealtime_MaxQuality;

Model::Model(
    const std::string &filename,
    const VertexLayout &vertexLayout,
    const ModelCreateInfo &createInfo,
    int flags)
    : filename(filename),
      vertexLayout(vertexLayout),
      createInfo(createInfo),
      flags(flags),
      indexCount(0),
      vertexCount(0),
      modelParts(std::vector<ModelPart>())
{
    loadFromFile();
}

void Model::loadFromFile()
{
    Assimp::Importer importer;

    auto scene = importer.ReadFile(filename.c_str(), flags);

    if (!scene)
    {
        std::string error = importer.GetErrorString();
        printf("ERROR: Failed to load the file with error: \n\t%s\n\n", error.c_str());
        return;
        // TODO: throw exception ? or fallback to default model ?
        //throw std::runtime_error("could not load model");
    }

    modelParts.resize(scene->mNumMeshes);

    std::vector<float> vertexBuffer;
    std::vector<uint32_t> indexBuffer;

    // Load meshes
    for (uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        auto mesh = scene->mMeshes[i];

        modelParts[i].vertexBase = vertexCount;
        modelParts[i].indexBase = indexCount;

        addVertices(mesh, vertexBuffer);
        modelParts[i].vertexCount = vertexCount - modelParts[i].vertexBase;

        addIndices(mesh, indexBuffer);
        modelParts[i].indexCount += indexCount - modelParts[i].indexBase;
    }
}

void Model::addVertices(const aiMesh *mesh, std::vector<float> &vertices)
{
    vertices.reserve(vertices.size() + mesh->mNumVertices * vertexLayout.getStride());

    for (uint32_t j = 0; j < mesh->mNumVertices; j++)
    {
        for (const auto &component : vertexLayout.getComponents())
        {
            switch (component)
            {
            case VERTEX_COMPONENT_POSITION:
            {
                const auto pos = mesh->mVertices[j];
                vertices.push_back(pos.x * createInfo.getScale().x + createInfo.getCenter().x);
                vertices.push_back(-pos.y * createInfo.getScale().y + createInfo.getCenter().y);
                vertices.push_back(pos.z * createInfo.getScale().z + createInfo.getCenter().z);
                /*dim.max.x = fmax(pos->x, dim.max.x);
                dim.max.y = fmax(pos->y, dim.max.y);
                dim.max.z = fmax(pos->z, dim.max.z);

                dim.min.x = fmin(pos->x, dim.min.x);
                dim.min.y = fmin(pos->y, dim.min.y);
                dim.min.z = fmin(pos->z, dim.min.z);*/
            }
            break;
            case VERTEX_COMPONENT_NORMAL:
            {
                const auto normal = mesh->mNormals[j];
                vertices.push_back(normal.x);
                vertices.push_back(-normal.y);
                vertices.push_back(normal.z);
            }
            break;
            case VERTEX_COMPONENT_UV:
            {
                const auto texCoord = mesh->HasTextureCoords(0)
                                          ? mesh->mTextureCoords[0][j]
                                          : aiVector3D(0.0f, 0.0f, 0.0f);
                vertices.push_back(texCoord.x * createInfo.getUvscale().s);
                vertices.push_back(texCoord.y * createInfo.getUvscale().t);
            }
            break;
            case VERTEX_COMPONENT_COLOR:
            {
                aiColor4D color = mesh->HasVertexColors(0)
                                      ? mesh->mColors[0][j]
                                      : aiColor4D(0.f, 0.f, 0.f, 0.f);
                vertices.push_back(color.r);
                vertices.push_back(color.g);
                vertices.push_back(color.b);
            }
            break;
            case VERTEX_COMPONENT_TANGENT:
            {
                const auto tangent = mesh->HasTangentsAndBitangents()
                                         ? mesh->mTangents[j]
                                         : aiVector3D(0.0f, 0.0f, 0.0f);

                vertices.push_back(tangent.x);
                vertices.push_back(tangent.y);
                vertices.push_back(tangent.z);
            }
            break;
            case VERTEX_COMPONENT_BITANGENT:
            {
                const auto biTangent = mesh->HasTangentsAndBitangents()
                                           ? mesh->mBitangents[j]
                                           : aiVector3D(0.0f, 0.0f, 0.0f);
                vertices.push_back(biTangent.x);
                vertices.push_back(biTangent.y);
                vertices.push_back(biTangent.z);
            }
            break;
            // Dummy components for padding
            case VERTEX_COMPONENT_DUMMY_FLOAT:
            {
                vertices.push_back(0.0f);
            }
            break;
            case VERTEX_COMPONENT_DUMMY_VEC4:
            {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
            break;
            };
        }
    }

    //dim.size = dim.max - dim.min;

    vertexCount += mesh->mNumVertices;
}

void Model::addIndices(const aiMesh *mesh, std::vector<uint32_t> &indices)
{
    auto indexBase = static_cast<uint32_t>(indices.size());
    for (uint32_t j = 0; j < mesh->mNumFaces; j++)
    {
        auto face = mesh->mFaces[j];
        if (face.mNumIndices != 3)
            continue;
        indices.push_back(indexBase + face.mIndices[0]);
        indices.push_back(indexBase + face.mIndices[1]);
        indices.push_back(indexBase + face.mIndices[2]);
        indexCount += 3;
    }
}

} // namespace TobiEngine