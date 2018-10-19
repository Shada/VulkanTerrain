#include "Platform.hpp"

namespace Tobi
{

class PlatformXcb : public Platform
{
  public:
    virtual ~PlatformXcb() = default;

    virtual Result initialize() override
    {
        return RESULT_SUCCESS;
    };
    virtual Result createWindow(const SwapChainDimensions &swapChainDimensions)
    {
        return RESULT_SUCCESS;
    }

  protected:
  private:
    virtual VkSurfaceKHR createSurface()
    {
        return VK_NULL_HANDLE;
    }
};

} // namespace Tobi