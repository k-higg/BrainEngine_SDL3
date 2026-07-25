#pragma once

#include <glm/glm.hpp>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

#include <memory>
#include <string>

namespace Brain {

typedef struct WindowSpecification {
    std::string     Title  = "Window";
    uint32_t        Width  = 1280;
    uint32_t        Height = 720;
    SDL_WindowFlags Flags  = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
} window_spec_t;

class Window {
  public:
    explicit Window(
        const WindowSpecification &specification = WindowSpecification());
    ~Window() = default;

    void Create();
    void Destroy();
    void Update();

    [[nodiscard]] uint32_t GetWidth() const { return m_specification.Width; }
    [[nodiscard]] uint32_t GetHeight() const { return m_specification.Height; }
    [[nodiscard]] glm::vec2 GetFramebufferSize() const;
    [[nodiscard]] glm::vec2 GetMousePos() const;
    [[nodiscard]] SDL_Window *GetHandle() const { return m_Window.get(); }
    [[nodiscard]] SDL_GLContext GetGLContext() const { return m_GLContext; }
    [[nodiscard]] bool ShouldClose(const SDL_EventType &event) const;

  private:
    window_spec_t                                       m_specification;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_Window;
    SDL_GLContext m_GLContext;
};

} // namespace brnCore
