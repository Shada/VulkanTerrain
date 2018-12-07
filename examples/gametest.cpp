#include <memory>
#include <iostream>

#include "framework/IContext.hpp"

#include "framework/TobiStatus.hpp"

// TODO: change so that platform is within context, and is not returned.
// Only use the context to interact with vulkan platform

namespace Tobi
{

class ApplicationStart
{
  public:
    ApplicationStart()
    {
        context = IContext::create();

        context->initialize();
    }
    ApplicationStart(const ApplicationStart &) = delete;
    ApplicationStart(ApplicationStart &&) = delete;
    ApplicationStart &operator=(const ApplicationStart &) & = delete;
    ApplicationStart &operator=(ApplicationStart &&) & = delete;
    ~ApplicationStart() = default;

    void run()
    {
        auto frameCount = static_cast<uint32_t>(0);
        auto startTime = 0.0;
        auto frameTime = 0.0;
        auto totalRunTime = 0.0;
        auto fpsReportTime = 0.0;

        auto maxFrameCount = static_cast<uint32_t>(100);
        auto useMaxFrameCount = false;

        while (context->getWindowStatus() == TobiStatus::TOBI_STATUS_RUNNING)
        {
            startTime = context->getCurrentTime();

            uint32_t swapChainIndex;
            auto result = context->acquireNextImage(swapChainIndex);

            if (FAILED(result))
            {
                LOGE("Unrecoverable swapchain error.\n");
                break;
            }

            result = context->update(frameTime);

            result = context->render();

            if (FAILED(result) && result != RESULT_ERROR_OUTDATED_SWAPCHAIN)
                break;

            auto endTime = context->getCurrentTime();
            frameTime = endTime - startTime;
            frameCount++;

            totalRunTime += frameTime;
            fpsReportTime += frameTime;

            if (fpsReportTime >= 1.0)
            {
                LOGI("FPS: %.3f\n", frameCount / fpsReportTime);
                frameCount = 0;
                fpsReportTime = 0.0;
                startTime = endTime;
            }
            if (useMaxFrameCount && (--maxFrameCount == 0))
                break;
        }
    }

  private:
    uint32_t model_id;
    std::unique_ptr<IContext> context;
};

} // namespace Tobi

int main()
{
    Tobi::ApplicationStart *applicationStart = new Tobi::ApplicationStart();

    applicationStart->run();

    delete applicationStart;
    applicationStart = nullptr;

    return EXIT_SUCCESS;
}
