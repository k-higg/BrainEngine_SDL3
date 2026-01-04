#include "AppLayer.h"

#include <SDL3/SDL_init.h>

AppLayer::AppLayer() { std::println("Created new AppLayer\n"); }

AppLayer::~AppLayer() {}

void AppLayer::OnEvent(SDL_Event &event) {}

void AppLayer::OnUpdate(float ts) {}

void AppLayer::OnRender() {}
