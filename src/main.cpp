#include <Cool/App/AppManager.h>
#include <Cool/Core/initialize.h>
#include <Cool/Window/WindowFactory.h>
#include "App.h"

void main()
{
    Cool::initialize();
    Cool::WindowFactory window_factory(4, 3);
    Cool::Window&       window = window_factory.make_main_window("You can change the window name in main.cpp", 1280, 720);
#if !defined(DEBUG)
    glfwMaximizeWindow(window.glfw());
#endif
    // App
    App              app(window);
    Cool::AppManager appManager(window, window_factory.window_manager(), app);
    appManager.run();
}