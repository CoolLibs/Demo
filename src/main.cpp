#include <Cool/App/AppManager.h>
#include <Cool/Core/initialize_and_shutdown.h>
#include <Cool/Window/WindowFactory.h>
#include "App.h"

void main()
{
    Cool::initialize();
#if defined(__COOL_APP_OPENGL)
    auto window_factory = Cool::WindowFactory{4, 3};
#elif defined(__COOL_APP_VULKAN)
    auto window_factory = Cool::WindowFactory{};
#endif
    auto& window = window_factory.make_main_window("You can change the window name in main.cpp", 1280, 720);
#if !defined(DEBUG)
    glfwMaximizeWindow(window.glfw());
#endif
    // App
    App              app(window);
    Cool::AppManager appManager(window, window_factory.window_manager(), app);
    appManager.run();
    Cool::shut_down();
}