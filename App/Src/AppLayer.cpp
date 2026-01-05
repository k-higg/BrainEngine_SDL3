#include "AppLayer.h"

#include "Engine/Core/Application.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>

AppLayer::AppLayer() {
    std::println("Created new AppLayer\n");
}

AppLayer::~AppLayer() {}

void AppLayer::OnEvent(SDL_Event &event) {}

void AppLayer::OnUpdate(float ts) {}

void AppLayer::OnRender() {
    auto commandBuffer = SDL_AcquireGPUCommandBuffer(
        brnCore::Application::Get().GetGpuDevice()->GetHandle());

    SDL_GPUTexture *swapchainTexture{};
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
            commandBuffer,
            brnCore::Application::Get().GetWindow()->GetHandle(),
            &swapchainTexture,
            nullptr,
            nullptr)) {
        SDL_LogError(brnCore::Application::APP_LOG_CATEGORY_GENERIC,
                     "Failed to acquire swapchain texture: %s",
                     SDL_GetError());
        exit(1);
    }

    if (swapchainTexture) {
        const std::array colorTargetInfos{SDL_GPUColorTargetInfo{
            .texture     = swapchainTexture,
            .clear_color = (SDL_FColor){1.0f, 0.0f, 0.0f, 1.0f},
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_STORE,
        }};
        const auto       renderPass = SDL_BeginGPURenderPass(commandBuffer,
                                                       colorTargetInfos.data(),
                                                       colorTargetInfos.size(),
                                                       nullptr);
        SDL_EndGPURenderPass(renderPass);
    }

    if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
        SDL_LogError(brnCore::Application::APP_LOG_CATEGORY_GENERIC,
                     "Failed to submit command buffer: %s",
                     SDL_GetError());
        exit(1);
    }
}
