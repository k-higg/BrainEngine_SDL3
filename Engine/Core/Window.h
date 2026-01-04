#pragma once

#include <glm/glm.hpp>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

#include <memory>
#include <string>

namespace brnCore {

struct WindowSpecification {
    std::string     Title  = "Window";
    uint32_t        Width  = 1280;
    uint32_t        Height = 720;
    SDL_WindowFlags Flags  = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
};

class Window {
  public:
    explicit Window(
        const WindowSpecification &specification = WindowSpecification());
    ~Window();

    void Create();
    void Destroy();
    void Update();

    glm::vec2 GetFramebufferSize() const;
    glm::vec2 GetMousePos() const;

    bool ShouldClose(const SDL_EventType &event) const;

    SDL_Window *GetHandle() const { return m_Window.get(); }

  private:
    WindowSpecification                                       m_specification;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_Window;
};

} // namespace brnCore
