#pragma once

#include <vector>

#include "Vertex.hpp"

namespace Tobi
{

class Model
{
  public:
    Model(const char *filename);
    Model(const Model &) = delete;
    Model(Model &&) = delete;
    Model &operator=(const Model &) & = delete;
    Model &operator=(Model &&) & = delete;
    ~Model() = default;

    const void *getVertexData() const { return vertices.data(); }
    const uint32_t getVertexCount() const { return vertices.size(); }
    const uint32_t getVertexDataSize() const { return sizeof(Vertex) * vertices.size(); }

    const void *getIndexData() const { return indices.data(); }
    const uint32_t getIndexCount() const { return indices.size(); }
    const uint32_t getIndexDataSize() const { return sizeof(uint32_t) * indices.size(); }

  private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    const char *filename;

    // have the buffer managers here ? and the buffer ids ?

    void initialize();
};

} // namespace Tobi