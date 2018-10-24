#include <memory>
#include <iostream>

#include "platform/xcb/PlatformXcb.hpp"
#include "framework/model/Model.hpp"
#include "framework/buffers/VertexBufferManager.hpp"

namespace Tobi
{

int run()
{
    auto platform = std::make_shared<PlatformXcb>();

    auto model = std::make_shared<Model>();

    auto vertexBuffer = std::make_shared<VertexBufferManager>(platform);

    vertexBuffer->createBuffer(
        model->getVertexData(),
        model->getVertexCount());

    return 1;
}

} // namespace Tobi

int main()
{
    return Tobi::run();
}