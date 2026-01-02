#include "Core/Window.h"

#include "Engine/Core/Application.h"

int main(int argc, char **argv) {
    brnCore::WindowSpecification windowSpec;
    windowSpec.Title  = "Brain";
    windowSpec.Width  = 1280;
    windowSpec.Height = 720;
    windowSpec.Flags  = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;

    brnCore::ApplicationSpecification appSpec;
    appSpec.appname       = "Brain";
    appSpec.version       = "1.0.0";
    appSpec.appidentifier = "com.brain.brian-app";
    appSpec.WindowSpec    = windowSpec;

    auto *app = new brnCore::Application{appSpec};

    app->Run();

    return 0;
}
