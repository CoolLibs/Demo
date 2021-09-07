#include <Cool/App/AppManager.h>
#include <Cool/Core/initialize_and_shutdown.h>
#include <Cool/Window/WindowFactory.h>
#include "App.h"

int main()
{
    Cool::initialize();
    auto  window_factory = Cool::WindowFactory{};
    auto& window         = window_factory.make_main_window({"You can change the window name in main.cpp"});
#if !defined(DEBUG)
    glfwMaximizeWindow(window.glfw());
#endif
    auto app         = App{window};
    auto app_manager = Cool::AppManager{window, window_factory.window_manager(), app};
    app_manager.run();
    Cool::shut_down();
}