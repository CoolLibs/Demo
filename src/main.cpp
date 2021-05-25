#include <Cool/Log/Log.h>
#include <Cool/App/AppManager.h>
#include <Cool/App/WindowFactory.h>

#include "App.h"

int main() {
    // Init Logging
    Cool::Log::Initialize();
	// Create window
    Cool::WindowFactory window_factory(4, 3);
    Cool::Window& window = window_factory.create("You can change the window name in main.cpp", 1280, 720);
#ifdef NDEBUG
	glfwMaximizeWindow(window.get());
#endif
	// App
	App app(window);
	Cool::AppManager appManager(window, app);
	appManager.run();
}