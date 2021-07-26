#include <Cool/App/AppManager.h>
#include <Cool/App/WindowFactory.h>
#include <Cool/Core/initialize.h>
#include "App.h"

int main()
{
    Cool::initialize();
    Cool::WindowFactory_Vulkan window_factory{};

    Cool::Window window = window_factory.make_window(
        "You can change the window title in main.cpp",
        1280,
        720);
#if !defined(DEBUG)
    glfwMaximizeWindow(window.glfw());
#endif

    App              app(window_factory.vku_framework(), window);
    Cool::AppManager appManager(window_factory.vku_framework(), window, app);
    appManager.run();
}