#include "Window.h"

#include <SDL3/SDL_log.h>

namespace Brain {

Window::Window(const WindowSpecification &specification)
    : m_specification(specification), m_Window(nullptr, &SDL_DestroyWindow) {}

void Window::Create() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    #ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    #endif

    m_Window.reset(SDL_CreateWindow(
        m_specification.Title.c_str(),
        m_specification.Width,
        m_specification.Height,
        m_specification.Flags | SDL_WINDOW_OPENGL));

    if (!m_Window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM,
                     "Failed to Create Window: %s",
                     SDL_GetError());
        exit(1);
    }

    m_GLContext = SDL_GL_CreateContext(m_Window.get());
    if (!m_GLContext) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to Create OpenGL Context");
        exit(1);
    }
    SDL_GL_MakeCurrent(m_Window.get(), m_GLContext);
    SDL_GL_SetSwapInterval(1); // vsync
}

void Window::Destroy() {
    if (m_GLContext) {
        SDL_GL_DestroyContext(m_GLContext);
        m_GLContext = nullptr;
    }
    m_Window.reset();
}

void Window::Update() { SDL_GL_SwapWindow(m_Window.get()); }

glm::vec2 Window::GetFramebufferSize() const {
    int width, height;
    SDL_GetWindowSizeInPixels(m_Window.get(), &width, &height);
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
