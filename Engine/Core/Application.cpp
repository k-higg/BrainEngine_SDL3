#include "Application.h"

#include <cassert>
#include <glm/glm.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace brnCore {
static Application *s_Application = nullptr;

Application::Application(const ApplicationSpecification &appSpec)
    : m_Window(nullptr), m_GpuDevice(nullptr, &SDL_DestroyGPUDevice),
      m_AppSpec(appSpec) {}

void Application::Run() {
    if (const auto result = Init(); result == SDL_APP_FAILURE) {
        return;
    }
}

void Application::Stop() {}

SDL_AppResult Application::Init() {
    SDL_SetAppMetadata(m_AppSpec.appname.c_str(),
                       m_AppSpec.version.c_str(),
                       m_AppSpec.appidentifier.c_str());

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Initialize SDL: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    m_Window = std::make_unique<Window>(m_AppSpec.WindowSpec);
    m_Window->Create();

    // m_Window.reset(SDL_CreateWindow(
    //     "Brain", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN));
    // if (!m_Window) {
    //     SDL_LogError(APP_LOG_CATEGORY_GENERIC,
    //                  "Failed to Create Window: %s",
    //                  SDL_GetError());
    //     return SDL_APP_FAILURE;
    // }

#pragma region Preferred GPU Driver Selection
    std::vector<std::string> gpuDrivers;

    const auto gpuDriverCount = SDL_GetNumGPUDrivers();
    gpuDrivers.reserve(gpuDriverCount);
    for (int i{}; i < gpuDriverCount; i++) {
        gpuDrivers.emplace_back(SDL_GetGPUDriver(i));
    }

    constexpr std::array preferredGpuDrivers{
        std::string{"vulkan"}, std::string{"metal"}, std::string{"direct3d12"}};

    std::string preferredDriver;

    for (const auto &gpuDriver : preferredGpuDrivers) {
        if (std::ranges::find(gpuDrivers, gpuDriver) != gpuDrivers.end()) {
            preferredDriver = gpuDriver;
            break;
        }
    }
#pragma endregion

    /*
     * Find the preferred driver above
     * if no preferred driver is found
     * use nullptr which auto selects the driver that the system supports
     */
    m_GpuDevice.reset(SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL |
            SDL_GPU_SHADERFORMAT_MSL,
        true,
        preferredDriver.size() ? preferredDriver.c_str() : nullptr));

    if (!m_GpuDevice) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Create a GPU Device: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_ClaimWindowForGPUDevice(m_GpuDevice.get(),
                                     m_Window->GetWindow())) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Claim Window for GPU Device: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

    if (SDL_WindowSupportsGPUPresentMode(
            m_GpuDevice.get(), m_Window->GetWindow(), presentMode)) {
        presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
    }

    SDL_SetGPUSwapchainParameters(m_GpuDevice.get(),
                                  m_Window->GetWindow(),
                                  SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                  presentMode);

    if (!SDL_ShowWindow(m_Window->GetWindow())) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Create Window: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::Iterate() {
    // TODO: refactor so that deltaTime is only calculated once for all methods
    // rather than per method
    float lastTime = GetTime();
    if (const auto result = OnUpdate(lastTime); result != SDL_APP_CONTINUE) {
        return result;
    }
    if (const auto result = OnRender(); result != SDL_APP_CONTINUE) {
        return result;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::Event(const SDL_Event *event) {
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return OnQuit();
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        if (SDL_GetWindowID(m_Window->GetWindow()) == event->window.windowID) {
            return SDL_APP_SUCCESS;
        }
    default:
        return SDL_APP_CONTINUE;
    }
}

void Application::Quit(const SDL_AppResult result) {
    SDL_WaitForGPUIdle(m_GpuDevice.get());
    SDL_ReleaseWindowFromGPUDevice(m_GpuDevice.get(), m_Window->GetWindow());
}

SDL_AppResult Application::OnUpdate(float lastTime) {
    float currentTime = GetTime();
    float deltaTime   = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
    for (const std::unique_ptr<Layer> &layer : m_LayerStack) {
        layer->OnUpdate(deltaTime);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::OnRender() {

    for (const std::unique_ptr<Layer> &layer : m_LayerStack) {
        layer->OnRender();
    }

    auto commandBuffer = SDL_AcquireGPUCommandBuffer(m_GpuDevice.get());

    SDL_GPUTexture *swapchainTexture{};

    if (!SDL_AcquireGPUSwapchainTexture(commandBuffer,
                                        m_Window->GetWindow(),
                                        &swapchainTexture,
                                        nullptr,
                                        nullptr)) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to acquire GPU Swapchain Texture: %s",
                     SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (swapchainTexture) {
        const std::array colorTargetInfos{SDL_GPUColorTargetInfo{
            .texture     = swapchainTexture,
            .clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 1.0f},
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_STORE}};
        const auto       renderPass = SDL_BeginGPURenderPass(commandBuffer,
                                                       colorTargetInfos.data(),
                                                       colorTargetInfos.size(),
                                                       nullptr);
        SDL_EndGPURenderPass(renderPass);
    }

    if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to submit command buffer: %s",
                     SDL_GetError());

        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}
SDL_AppResult Application::OnQuit() { return SDL_APP_SUCCESS; }

Application &Application::Get() {
    assert(s_Application);
    return *s_Application;
}

float Application::GetTime() { return (float)SDL_GetTicks(); }

} // namespace brnCore
