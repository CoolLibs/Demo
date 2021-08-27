#include <Cool/App/AppManager.h>
#include <Cool/Core/initialize_and_shutdown.h>
#include <Cool/Window/WindowFactory.h>
#include "App.h"

void main()
{
    Cool::initialize();
    auto  window_factory = Cool::WindowFactory{4, 3};
    auto& window         = window_factory.make_main_window("You can change the window name in main.cpp", 1280, 720);
#if !defined(DEBUG)
    glfwMaximizeWindow(window.glfw());
#endif
    // App
    App              app(window);
    Cool::AppManager appManager(window, window_factory.window_manager(), app);
    appManager.run();
    Cool::shut_down();
}