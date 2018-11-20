#pragma once

#include <map>
#include <memory>

#include "Model.hpp"
#include "../buffers/VertexBufferManager.hpp"
#include "../buffers/IndexBufferManager.hpp"

namespace Tobi
{

class ModelManager
{
  public:
    ModelManager(std::shared_ptr<VertexBufferManager> vertexBufferManager,
                 std::shared_ptr<IndexBufferManager> indexBufferManager);
    ModelManager(const ModelManager &) = delete;
    ModelManager(ModelManager &&) = delete;
    ModelManager &operator=(const ModelManager &) & = delete;
    ModelManager &operator=(ModelManager &&) & = delete;
    ~ModelManager() = default;

    uint32_t loadModel(const char *filename);

    const auto &getModel(uint32_t index) { return models[index]; }
    const auto &getVertexBufferIndex(uint32_t index) { return vertexBufferIndices[index]; }
    const auto &getIndexBufferIndex(uint32_t index) { return indexBufferIndices[index]; }
    //const auto &getModel(const char *modelName) { return modelMap[modelNameMap[modelName]]; }

  private:
    std::shared_ptr<VertexBufferManager> vertexBufferManager;
    std::shared_ptr<IndexBufferManager> indexBufferManager;

    std::vector<std::shared_ptr<Model>> models;
    std::vector<const char *> modelFileNames;

    std::vector<uint32_t> vertexBufferIndices;
    std::vector<uint32_t> indexBufferIndices;
};

} // namespace Tobi