#include <memory>
#include <iostream>

#include "framework/Context.hpp"

#include "framework/Status.hpp"

// TODO: change so that platform is within context, and is not returned.
// Only use the context to interact with vulkan platform

namespace Tobi
{

class ApplicationStart
{
  public:
    ApplicationStart()
    {
        context = std::make_shared<Context>();

        context->initialize();

        model_id = context->loadModel("modelname");
    }
    ~ApplicationStart() = default;

    int run()
    {
        auto frameCount = static_cast<uint32_t>(0);
        auto startTime = context->getCurrentTime();

        auto maxFrameCount = static_cast<uint32_t>(100);
        auto useMaxFrameCount = false;

        while (context->getWindowStatus() == Status::STATUS_RUNNING)
        {
            uint32_t swapChainIndex;
            auto result = context->acquireNextImage(swapChainIndex);

            if (FAILED(result))
            {
                LOGE("Unrecoverable swapchain error.\n");
                break;
            }
            result = context->render(0.0166f);

            if (FAILED(result) && result != RESULT_ERROR_OUTDATED_SWAPCHAIN)
                break;

            frameCount++;
            if (frameCount == 100)
            {
                auto endTime = context->getCurrentTime();
                LOGI("FPS: %.3f\n", frameCount / (endTime - startTime));
                frameCount = 0;
                startTime = endTime;
            }
            if (useMaxFrameCount && (--maxFrameCount == 0))
                break;
        }

        return 0;
    }

  private:
    uint32_t model_id;
    std::shared_ptr<Context> context;
};

} // namespace Tobi

int main()
{
    Tobi::ApplicationStart *applicationStart = new Tobi::ApplicationStart();

    applicationStart->run();

    delete applicationStart;
    applicationStart = nullptr;

    return 0;
}