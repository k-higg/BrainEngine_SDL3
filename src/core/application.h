#pragma once

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_gpu.h>

#include <memory>

class Application {
  public:
    Application(int argc, char **argv);
    ~Application() = default;

    SDL_AppResult Init();
    SDL_AppResult Iterate();
    SDL_AppResult Event(const SDL_Event *event);
    void          Quit(const SDL_AppResult result);

    enum AppLogCategory {
        APP_LOG_CATEGORY_GENERIC = SDL_LOG_CATEGORY_CUSTOM,
    };

  private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>       m_Window;
    std::unique_ptr<SDL_GPUDevice, decltype(&SDL_DestroyGPUDevice)> m_GpuDevice;

    SDL_AppResult OnUpdate();
    SDL_AppResult OnRender();
    SDL_AppResult OnQuit();
};
