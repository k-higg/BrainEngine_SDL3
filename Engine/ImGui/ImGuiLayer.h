#pragma once

#include "Engine/Core/Layer.h"

namespace Brain {

    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer() = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(SDL_Event &event) override;
        void OnRender() override;

        void Begin();
        void End();
        void BlockEvents(bool block) { m_BlockEvents = block; }
        void SetDarkThemeColors();

        uint32_t GetActiveWidgetID() const;

    private:
        bool m_BlockEvents = false;
    };

}