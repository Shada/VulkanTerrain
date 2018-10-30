#include "ModelManager.hpp"
#include "Common.hpp"

namespace Tobi
{

ModelManager::ModelManager()
    : modelMap(std::map<uint32_t, std::shared_ptr<Model>>()),
      modelNameMap(std::map<const char *, uint32_t>()),
      idCounter(1)
{
}

uint32_t ModelManager::loadModel(const char *filename)
{
    if (modelNameMap[filename] != 0)
    {
        LOGW("Model already loaded\n");
        return modelNameMap[filename];
    }
    modelMap[idCounter] = std::make_shared<Model>();
    modelNameMap[filename] = idCounter;

    return idCounter++;
}

} // namespace Tobi