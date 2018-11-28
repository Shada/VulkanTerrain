#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "ModelCreateInfo.hpp"
#include "ModelPart.hpp"
#include "VertexLayout.hpp"

namespace TobiEngine
{

class Model
{
  public:
    Model(const std::string &filename,
          const VertexLayout &vertexLayout,
          const ModelCreateInfo &createInfo,
          int flags = defaultFlags);
    ~Model() = default;

    /**
		* Loads a 3D model from a file into Vulkan buffers
		*
		* @param filename File to load (must be a model format supported by ASSIMP)
		* @param layout Vertex layout components (position, normals, tangents, etc.)
		* @param createInfo MeshCreateInfo structure for load time settings like scale, center, etc.
		* @param (Optional) flags ASSIMP model loading flags
		*/
    void loadFromFile();

    const auto &getFileName() const { return filename; }
    const auto &getIndexCount() const { return indexCount; }
    const auto &getVertexCount() const { return vertexCount; }

  private:
    static const int defaultFlags;

    std::string filename;
    VertexLayout vertexLayout;
    ModelCreateInfo createInfo;
    int flags;

    //VkDevice device = nullptr;
    //vks::Buffer vertices;
    //vks::Buffer indices;
    uint32_t indexCount;
    uint32_t vertexCount;

    std::vector<ModelPart> modelParts;

    void addVertices(const aiMesh *mesh, std::vector<float> &vertices);
    void addIndices(const aiMesh *mesh, std::vector<uint32_t> &indices);
};

} // namespace TobiEngine