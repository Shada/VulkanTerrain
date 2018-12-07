#pragma once

#include <memory>
#include <vector>

#include "Common.hpp"
#include "TobiStatus.hpp"

namespace Tobi
{

/// @brief The Context is the primary way for samples to interact
/// with the swapchain and get rendered images to screen.
class IContext
{
  public:
    virtual ~IContext() = default;
    
    static std::unique_ptr<IContext> create();

    virtual Result initialize() = 0;

    virtual Result update(float time) = 0;
    virtual Result render() = 0;

    virtual uint32_t loadModel(const char *filename) = 0;

    virtual Result acquireNextImage(uint32_t &swapChainIndex) = 0;

    virtual double getCurrentTime() = 0;

    virtual TobiStatus getWindowStatus() = 0;
};

} // namespace Tobi
