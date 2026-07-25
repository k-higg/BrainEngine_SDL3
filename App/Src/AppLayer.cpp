#include "AppLayer.h"

#include "Engine/Core/Application.h"

#include <imgui.h>

AppLayer::AppLayer() { std::println("Created new AppLayer\n"); }

AppLayer::~AppLayer() {}

void AppLayer::OnEvent(SDL_Event &event) {}

void AppLayer::OnUpdate(float ts) {}

void AppLayer::OnRender() {
    //ImGui::Begin("Brain App Layer");
    //ImGui::Text("Hello from AppLayer!");
    //ImGui::End();
}
