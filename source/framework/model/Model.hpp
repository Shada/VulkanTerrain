#pragma once

#include <vector>

#include "Vertex.hpp"

namespace Tobi
{

class Model
{
  public:
    //TODO: load models from file
    Model(const char *filename);
    Model(const Model &) = delete;
    Model(Model &&) = delete;
    Model &operator=(const Model &) & = delete;
    Model &operator=(Model &&) & = delete;
    ~Model() = default;

    const void *getVertexData() const { return vertices.data(); }
    const uint32_t getVertexCount() const { return vertices.size(); }
    const uint32_t getVertexDataSize() const { return sizeof(Vertex) * vertices.size(); }

  private:
    std::vector<Vertex> vertices;
    const char *filename;

    void initialize();
};

} // namespace Tobi