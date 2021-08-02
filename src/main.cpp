#include <Cool/App/AppManager.h>
#include <Cool/App/WindowFactory.h>
#include <Cool/Core/initialize.h>
#include "App.h"

void main()
{
    Cool::initialize();
    // Init Glfw and set OpenGL version
#ifndef __APPLE__
    Cool::WindowFactory window_factory(4, 3);
#else
    Cool::WindowFactory window_factory(3, 3); // OpenGL > 3.3 is not supported on MacOS
#endif
    // Create the main window and init OpenGL
    Cool::Window& mainWindow = window_factory.create("You can change the window name in main.cpp", 1280, 720);
#ifndef DEBUG
    glfwMaximizeWindow(mainWindow.get());
#endif
    // App
    App              app(mainWindow);
    Cool::AppManager appManager(mainWindow, app);
    appManager.run();
}