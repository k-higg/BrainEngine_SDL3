#pragma once

#include "Engine/Core/Layer.h"

#include <SDL3/SDL_events.h>

#include <print>

class AppLayer : public brnCore::Layer {
  public:
    AppLayer();
    virtual ~AppLayer();

    virtual void OnEvent(SDL_Event &event) override;
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
};
