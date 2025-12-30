#include "application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

#include <algorithm>
#include <array>
#include <string>
#include <vector>

Application::Application(int argc, char **argv)
    : m_Window(nullptr, &SDL_DestroyWindow),
      m_GpuDevice(nullptr, &SDL_DestroyGPUDevice) {}

SDL_AppResult Application::Init() {
    SDL_SetAppMetadata(
        "BrainEngine SDL", "1.0.0", "com.brainengine.brainengine-sdl");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Initialize SDL: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }
    m_Window.reset(SDL_CreateWindow(
        "BrainEngine", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN));
    if (!m_Window) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Create Window: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

#pragma region Preferred GPU Driver Selection
    std::vector<std::string> gpuDrivers;

    const auto gpuDriverCount = SDL_GetNumGPUDrivers();
    gpuDrivers.reserve(gpuDriverCount);
    for (int i{}; i < gpuDriverCount; i++) {
        gpuDrivers.emplace_back(SDL_GetGPUDriver(i));
    }

    SDL_Log("Supported GPU Drivers:");
    for (int i{}; i < SDL_GetNumGPUDrivers(); i++) {
        SDL_Log("    %s", SDL_GetGPUDriver(i));
    }

    constexpr std::array preferredGpuDrivers{
        std::string{"vulkan"}, std::string{"metal"}, std::string{"direct3d12"}};

    std::string preferredDriver;

    for (const auto &gpuDriver : preferredGpuDrivers) {
        if (std::ranges::find(gpuDrivers, gpuDriver) != gpuDrivers.end()) {
            SDL_Log("Using preferred GPU Driver: %s", gpuDriver.c_str());
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

    SDL_Log("Selected GPU Driver: %s",
            SDL_GetGPUDeviceDriver(m_GpuDevice.get()));

    if (!SDL_ClaimWindowForGPUDevice(m_GpuDevice.get(), m_Window.get())) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Claim Window for GPU Device: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

    if (SDL_WindowSupportsGPUPresentMode(
            m_GpuDevice.get(), m_Window.get(), presentMode)) {
        presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
    }

    SDL_SetGPUSwapchainParameters(m_GpuDevice.get(),
                                  m_Window.get(),
                                  SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                  presentMode);

    if (!SDL_ShowWindow(m_Window.get())) {
        SDL_LogError(APP_LOG_CATEGORY_GENERIC,
                     "Failed to Create Window: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::Iterate() {
    if (const auto result = OnUpdate(); result != SDL_APP_CONTINUE) {
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
        if (SDL_GetWindowID(m_Window.get()) == event->window.windowID) {
            return SDL_APP_SUCCESS;
        }
    default:
        return SDL_APP_CONTINUE;
    }
}

void Application::Quit(const SDL_AppResult result) {
    SDL_WaitForGPUIdle(m_GpuDevice.get());
    SDL_ReleaseWindowFromGPUDevice(m_GpuDevice.get(), m_Window.get());
}

SDL_AppResult Application::OnUpdate() { return SDL_APP_CONTINUE; }
SDL_AppResult Application::OnRender() {
    auto commandBuffer = SDL_AcquireGPUCommandBuffer(m_GpuDevice.get());

    SDL_GPUTexture *swapchainTexture{};

    if (!SDL_AcquireGPUSwapchainTexture(commandBuffer,
                                        m_Window.get(),
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
            .clear_color = (SDL_FColor){1.0f, 0.0f, 0.0f, 1.0f},
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
