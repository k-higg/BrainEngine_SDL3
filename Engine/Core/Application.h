#pragma once

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_gpu.h>

#include <memory>
#include <vector>

#include "Core/Layer.h"
#include "Core/Window.h"

namespace brnCore {

struct ApplicationSpecification {
    std::string         appname       = "BrianEngine SDL";
    std::string         version       = "1.0.0";
    std::string         appidentifier = "com.brainengine.brainengine-sdl";
    WindowSpecification WindowSpec;
};

class Application {
  public:
    enum AppLogCategory {
        APP_LOG_CATEGORY_GENERIC = SDL_LOG_CATEGORY_CUSTOM,
    };

    Application(
        const ApplicationSpecification &appSpec = ApplicationSpecification());
    ~Application() = default;

    void Run();
    void Stop();
    void RaiseEvent(SDL_Event &event);

    SDL_AppResult Init();
    SDL_AppResult Iterate();
    SDL_AppResult Event(const SDL_Event *event);
    void          Quit(const SDL_AppResult result);

    template <typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
    void PushLayer() {
        m_LayerStack.push_back(std::make_unique<TLayer>());
    }

    template <typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
    TLayer *GetLayer() {
        for (const auto &layer : m_LayerStack) {
            if (auto casted = dynamic_cast<TLayer *>(layer.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    static Application &Get();
    static float        GetTime();

  private:
    ApplicationSpecification                                        m_AppSpec;
    std::unique_ptr<Window>                                         m_Window;
    std::unique_ptr<SDL_GPUDevice, decltype(&SDL_DestroyGPUDevice)> m_GpuDevice;

    std::vector<std::unique_ptr<Layer>> m_LayerStack;

    SDL_AppResult OnUpdate(float lastTime);
    SDL_AppResult OnRender();
    SDL_AppResult OnQuit();
};
} // namespace brnCore
