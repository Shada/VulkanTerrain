#include "ModelManager.hpp"

#include "framework/Common.hpp"

namespace Tobi
{

ModelManager::ModelManager(std::shared_ptr<VertexBufferManager> vertexBufferManager,
                           std::shared_ptr<IndexBufferManager> indexBufferManager)
    : vertexBufferManager(vertexBufferManager),
      indexBufferManager(indexBufferManager),
      models(std::vector<std::shared_ptr<Model>>()),
      modelFileNames(std::vector<const char *>()),
      vertexBufferIndices(std::vector<uint32_t>()),
      indexBufferIndices(std::vector<uint32_t>())
{
}

uint32_t ModelManager::loadModel(const char *filename)
{
    auto it = std::find(modelFileNames.begin(), modelFileNames.end(), filename);

    if (it != modelFileNames.end())
    {
        LOGW("Model already loaded\n");
        return (it - modelFileNames.begin());
    }

    models.push_back(std::make_shared<Model>(filename));
    modelFileNames.push_back(filename);

    // TODO: this should be refactored. it is pretty ugly. find a more elegant way to connect buffer with model
    auto vertexBufferIndex = vertexBufferManager->createBuffer(models.back()->getVertexData(),
                                                               models.back()->getVertexDataSize());
    auto indexBufferIndex = indexBufferManager->createBuffer(models.back()->getIndexData(),
                                                             models.back()->getIndexDataSize());

    vertexBufferIndices.push_back(vertexBufferIndex);
    indexBufferIndices.push_back(indexBufferIndex);

    return models.size() - 1;
}

} // namespace Tobi
