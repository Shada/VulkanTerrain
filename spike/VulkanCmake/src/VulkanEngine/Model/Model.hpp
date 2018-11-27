#pragma once

#include <cstdint>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "ModelCreateInfo.hpp"
#include "VertexLayout.hpp"
#include "ModelPart.hpp"

namespace TobiEngine
{

class Model
{
  public:
    Model();
    ~Model() = default;

    /**
		* Loads a 3D model from a file into Vulkan buffers
		*
		* @param filename File to load (must be a model format supported by ASSIMP)
		* @param layout Vertex layout components (position, normals, tangents, etc.)
		* @param createInfo MeshCreateInfo structure for load time settings like scale, center, etc.
		* @param (Optional) flags ASSIMP model loading flags
		*/
    bool loadFromFile(
        const std::string &filename,
        VertexLayout layout,
        ModelCreateInfo *createInfo,
        const int flags = defaultFlags);

    uint32_t getIndexCount() { return indexCount; }
    uint32_t getVertexCount() { return vertexCount; }

  private:
    //VkDevice device = nullptr;
    //vks::Buffer vertices;
    //vks::Buffer indices;
    uint32_t indexCount = 0;
    uint32_t vertexCount = 0;

    std::vector<ModelPart> modelParts;

    static const int defaultFlags;
};

} // namespace TobiEngine