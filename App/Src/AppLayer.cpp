#include "AppLayer.h"

#include "Engine/Core/Application.h"

#include <imgui.h>
#include <imgui_internal.h>

AppLayer::AppLayer() { std::println("Created new AppLayer\n"); }

AppLayer::~AppLayer() {}

void AppLayer::OnEvent(SDL_Event &event) {}

void AppLayer::OnUpdate(float ts) {}

void AppLayer::OnRender() {
    ImGui::Begin("Brain Test");
    ImGui::Text("Testing");
    ImGui::End();
}
