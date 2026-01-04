#include "Window.h"

#include <SDL3/SDL_log.h>

namespace brnCore {

Window::Window(const WindowSpecification &specification)
    : m_specification(specification), m_Window(nullptr, &SDL_DestroyWindow) {}

Window::~Window() { Destroy(); }

void Window::Create() {
    m_Window.reset(SDL_CreateWindow(m_specification.Title.c_str(),
                                    m_specification.Width,
                                    m_specification.Height,
                                    m_specification.Flags));
    if (!m_Window) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM,
                     "Failed to Create Window: %s",
                     SDL_GetError());
        exit(1);
    }
}

void Window::Destroy() {
    if (m_Window) {
        SDL_DestroyWindow(m_Window.get());
    }
    m_Window = nullptr;
}

void Window::Update() { SDL_GL_SwapWindow(m_Window.get()); }

glm::vec2 Window::GetFramebufferSize() const {
    int width, height;
    SDL_GetWindowSize(m_Window.get(), &width, &height);
    return {width, height};
}

glm::vec2 Window::GetMousePos() const {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y};
}

bool Window::ShouldClose(const SDL_EventType &event) const {
    return event == SDL_EVENT_WINDOW_CLOSE_REQUESTED ? true : false;
}

} // namespace brnCore
