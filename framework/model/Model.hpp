#include <vector>

#include "Vertex.hpp"

namespace Tobi
{

class Model
{
  public:
    //TODO: load models from file
    Model();
    ~Model();

    const void *getVertexData() const { return vertices.data(); }
    const uint32_t getVertexCount() const { return vertices.size(); }

  private:
    std::vector<Vertex> vertices;

    void initialize();
};

} // namespace Tobi