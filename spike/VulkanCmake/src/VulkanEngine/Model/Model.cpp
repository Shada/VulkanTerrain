#include "Model.hpp"

#include <stdexcept>
#include <string>

namespace TobiEngine
{

const int Model::defaultFlags =
    aiProcess_FlipWindingOrder |
    aiProcess_Triangulate |
    aiProcess_PreTransformVertices |
    aiProcess_CalcTangentSpace |
    aiProcess_GenSmoothNormals |
    aiProcessPreset_TargetRealtime_MaxQuality;

Model::Model()
    : indexCount(0),
      vertexCount(0),
      modelParts(std::vector<ModelPart>())
{
}

bool Model::loadFromFile(const std::string &filename, VertexLayout layout, ModelCreateInfo *createInfo, const int flags)
{
    const aiScene *scene;
    Assimp::Importer importer;

    scene = importer.ReadFile(filename.c_str(), flags);

    if (!scene)
    {
        std::string error = importer.GetErrorString();
        printf("ERROR: Failed to load the file with error: \n\t%s\n\n", error.c_str());
        return false;
    }

    modelParts.clear();
    modelParts.resize(scene->mNumMeshes);

    glm::vec3 scale(1.0f);
    glm::vec2 uvscale(1.0f);
    glm::vec3 center(0.0f);
    if (createInfo)
    {
        scale = createInfo->getScale();
        uvscale = createInfo->getUvscale();
        center = createInfo->getCenter();
    }

    std::vector<float> vertexBuffer;
    std::vector<uint32_t> indexBuffer;

    vertexCount = 0;
    indexCount = 0;
    // Load meshes
    for (uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh *mesh = scene->mMeshes[i];

        modelParts[i] = {};
        modelParts[i].vertexBase = vertexCount;
        modelParts[i].indexBase = indexCount;

        vertexCount += scene->mMeshes[i]->mNumVertices;

        aiColor3D color(0.f, 0.f, 0.f);
        scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            const aiVector3D *pos = &(mesh->mVertices[j]);
            const aiVector3D *normal = &(mesh->mNormals[j]);
            const aiVector3D *texCoord = (mesh->HasTextureCoords(0)) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
            const aiVector3D *tangent = (mesh->HasTangentsAndBitangents()) ? &(mesh->mTangents[j]) : &Zero3D;
            const aiVector3D *biTangent = (mesh->HasTangentsAndBitangents()) ? &(mesh->mBitangents[j]) : &Zero3D;

            for (auto &component : layout.getComponents())
            {
                switch (component)
                {
                case VERTEX_COMPONENT_POSITION:
                    vertexBuffer.push_back(pos->x * scale.x + center.x);
                    vertexBuffer.push_back(-pos->y * scale.y + center.y);
                    vertexBuffer.push_back(pos->z * scale.z + center.z);
                    break;
                case VERTEX_COMPONENT_NORMAL:
                    vertexBuffer.push_back(normal->x);
                    vertexBuffer.push_back(-normal->y);
                    vertexBuffer.push_back(normal->z);
                    break;
                case VERTEX_COMPONENT_UV:
                    vertexBuffer.push_back(texCoord->x * uvscale.s);
                    vertexBuffer.push_back(texCoord->y * uvscale.t);
                    break;
                case VERTEX_COMPONENT_COLOR:
                    vertexBuffer.push_back(color.r);
                    vertexBuffer.push_back(color.g);
                    vertexBuffer.push_back(color.b);
                    break;
                case VERTEX_COMPONENT_TANGENT:
                    vertexBuffer.push_back(tangent->x);
                    vertexBuffer.push_back(tangent->y);
                    vertexBuffer.push_back(tangent->z);
                    break;
                case VERTEX_COMPONENT_BITANGENT:
                    vertexBuffer.push_back(biTangent->x);
                    vertexBuffer.push_back(biTangent->y);
                    vertexBuffer.push_back(biTangent->z);
                    break;
                // Dummy components for padding
                case VERTEX_COMPONENT_DUMMY_FLOAT:
                    vertexBuffer.push_back(0.0f);
                    break;
                case VERTEX_COMPONENT_DUMMY_VEC4:
                    vertexBuffer.push_back(0.0f);
                    vertexBuffer.push_back(0.0f);
                    vertexBuffer.push_back(0.0f);
                    vertexBuffer.push_back(0.0f);
                    break;
                };
            }

            /*dim.max.x = fmax(pPos->x, dim.max.x);
            dim.max.y = fmax(pPos->y, dim.max.y);
            dim.max.z = fmax(pPos->z, dim.max.z);

            dim.min.x = fmin(pPos->x, dim.min.x);
            dim.min.y = fmin(pPos->y, dim.min.y);
            dim.min.z = fmin(pPos->z, dim.min.z);*/
        }

        //dim.size = dim.max - dim.min;

        modelParts[i].vertexCount = mesh->mNumVertices;

        uint32_t indexBase = static_cast<uint32_t>(indexBuffer.size());
        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            const aiFace &face = mesh->mFaces[j];
            if (face.mNumIndices != 3)
                continue;
            indexBuffer.push_back(indexBase + face.mIndices[0]);
            indexBuffer.push_back(indexBase + face.mIndices[1]);
            indexBuffer.push_back(indexBase + face.mIndices[2]);
            modelParts[i].indexCount += 3;
            indexCount += 3;
        }
    }
    return true;
}

} // namespace TobiEngine