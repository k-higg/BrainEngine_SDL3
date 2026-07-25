#include "Application.h"

#include <glm/glm.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <cassert>
#include <string>

#include "Timestep.h"
#include "ImGui/ImGuiLayer.h"


namespace Brain {
static Application *s_Application = nullptr;

Application::Application(const ApplicationSpecification &appSpec)
    : m_Window(nullptr), m_GpuDevice(nullptr), m_AppSpec(appSpec) {
    s_Application = this;
}

SDL_AppResult Application::Init() {
    SDL_SetAppMetadata(m_AppSpec.appname.c_str(),
                       m_AppSpec.version.c_str(),
                       m_AppSpec.app_identifier.c_str());

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Initialize SDL: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    m_Window = std::make_shared<Window>(m_AppSpec.WindowSpec);
    m_Window->Create();

    m_GpuDevice = std::make_shared<Device>();
    m_GpuDevice->Create();

    if (!SDL_ShowWindow(m_Window->GetHandle())) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Create Window: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    PushLayer<ImGuiLayer>();

    return SDL_APP_CONTINUE;
}

void Application::Run() {
    if (const auto result = Init(); result == SDL_APP_FAILURE) {
        return;
    }

    float lastTime = GetTime();

    bool b_Run = true;
    while (b_Run) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // TODO: obviously needs to be reworked because it could return
            // success for things other than wanting to close the window/quit.
            // Unless this is only for application events.
            if (const auto result = Event(&event); result == SDL_APP_SUCCESS) {
                b_Run = false;
            }

            for (const auto &layer : m_LayerStack) {
                layer->OnEvent(event);
            }
        }

        const float currentTime = GetTime();

        const float    deltaTime = (currentTime - lastTime) / 1000.0f;
        Timestep ts(deltaTime);
        lastTime = currentTime;

        for (const std::unique_ptr<Layer> &layer : m_LayerStack) {
            layer->OnUpdate(ts);
        }

        // NOTE: rendering can be done elsewhere (eg. render thread)
        for (const std::unique_ptr<Layer> &layer : m_LayerStack) {
            layer->OnRender();
        }

        m_Window->Update();
    }

    Stop();
}

// TODO: Either delete this function or replace Quit
void Application::Stop() { Quit(SDL_APP_SUCCESS); }

SDL_AppResult Application::Event(const SDL_Event *event) {
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return OnQuit();
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        if (SDL_GetWindowID(m_Window->GetHandle()) == event->window.windowID) {
            return SDL_APP_SUCCESS;
        }
    default:
        return SDL_APP_CONTINUE;
    }
}

void Application::Quit(const SDL_AppResult result) {
    m_GpuDevice->Destroy();
    m_Window->Destroy();
}

SDL_AppResult Application::OnQuit() { return SDL_APP_SUCCESS; }

Application &Application::Get() {
    assert(s_Application);
    return *s_Application;
}

float Application::GetTime() { return (float)SDL_GetTicks(); }

} // namespace brnCore
