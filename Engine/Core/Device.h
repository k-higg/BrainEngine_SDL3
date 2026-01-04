#pragma once

#include <SDL3/SDL_gpu.h>

#include <memory>

namespace brnCore {

class Device {
  public:
    Device();
    ~Device();

    void Create();
    void Destroy();

    SDL_GPUDevice *GetHandle() const { return m_GpuDevice.get(); }

  private:
    std::unique_ptr<SDL_GPUDevice, decltype(&SDL_DestroyGPUDevice)> m_GpuDevice;
};
} // namespace brnCore
