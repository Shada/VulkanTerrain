#pragma once

#include <map>
#include <memory>

#include "Model.hpp"

namespace Tobi
{

class ModelManager
{
  public:
    ModelManager();
    ModelManager(const ModelManager &) = delete;
    ModelManager(ModelManager &&) = delete;
    ModelManager &operator=(const ModelManager &) & = delete;
    ModelManager &operator=(ModelManager &&) & = delete;
    ~ModelManager() = default;

    uint32_t loadModel(const char *filename);

    const auto &getModel(uint32_t index) { return modelMap[index]; }
    const auto &getModel(const char *modelName) { return modelMap[modelNameMap[modelName]]; }

  private:
    std::map<uint32_t, std::shared_ptr<Model>> modelMap;
    std::map<const char *, uint32_t> modelNameMap;

    uint32_t idCounter = 0;
};

} // namespace Tobi