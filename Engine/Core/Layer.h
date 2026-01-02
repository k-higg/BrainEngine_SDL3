#pragma once

#include <SDL3/SDL_events.h>

#include <memory>

namespace brnCore {
class Layer {
  public:
    virtual ~Layer() = default;

    virtual void OnEvent(SDL_Event &event) {}
    virtual void OnUpdate(float ts) {}
    virtual void OnRender() {}

    template <std::derived_from<Layer> T, typename... Args>
    void TransitionTo(Args &&...args) {
        QueueTransition(
            std::move(std::make_unique<T>(std::forward<Args>(args)...)));
    }

  private:
    void QueueTransition(std::unique_ptr<Layer> layer);
};
} // namespace brnCore
