#include "Device.h"

#include "Engine/Core/Application.h"

#include <SDL3/SDL_gpu.h>
#include <algorithm>
#include <vector>

namespace brnCore {

Device::Device() : m_GpuDevice(nullptr, &SDL_DestroyGPUDevice) {}
Device::~Device() { Destroy(); }

void Device::Create() {
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
        SDL_LogError(brnCore::Application::Get().APP_LOG_CATEGORY_GENERIC,
                     "Failed to Create a GPU Device: %s",
                     SDL_GetError());
        exit(1);
        // return SDL_APP_FAILURE;
    }

    if (!SDL_ClaimWindowForGPUDevice(
            m_GpuDevice.get(),
            brnCore::Application::Get().GetWindow()->GetHandle())) {
        SDL_LogError(brnCore::Application::Get().APP_LOG_CATEGORY_GENERIC,
                     "Failed to Claim Window for GPU Device: %s",
                     SDL_GetError());
        exit(1);
        // return SDL_APP_FAILURE;
    }

    SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

    if (SDL_WindowSupportsGPUPresentMode(
            m_GpuDevice.get(),
            brnCore::Application::Get().GetWindow()->GetHandle(),
            presentMode)) {
        presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
    }

    SDL_SetGPUSwapchainParameters(
        m_GpuDevice.get(),
        brnCore::Application::Get().GetWindow()->GetHandle(),
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        SDL_GPU_PRESENTMODE_VSYNC);
}

void Device::Destroy() {
    if (m_GpuDevice) {
        SDL_WaitForGPUIdle(m_GpuDevice.get());
        SDL_ReleaseWindowFromGPUDevice(
            m_GpuDevice.get(),
            brnCore::Application::Get().GetWindow()->GetHandle());
    }
    m_GpuDevice = nullptr;
}

} // namespace brnCore
