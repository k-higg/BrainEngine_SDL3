#include "AppLayer.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"

int main(int argc, char **argv) {
    Brain::window_spec_t windowSpec;
    windowSpec.Title  = "Brain";
    windowSpec.Width  = 1280;
    windowSpec.Height = 720;
    windowSpec.Flags  = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;

    Brain::application_spec_t appSpec;
    appSpec.appname       = "Brain";
    appSpec.version       = "1.0.0";
    appSpec.app_identifier = "com.brain.brian-app";
    appSpec.WindowSpec    = windowSpec;

    Brain::Application app(appSpec);
    app.PushLayer<AppLayer>();
    app.Run();

    return 0;
}
