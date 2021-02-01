#include <Cool/Log/Log.h>
#include <Cool/App/AppManager.h>

#include "App.h"

int main() {
	Cool::Log::Initialize();
	Cool::AppManager appManager("You can change the window name in main.cpp");
	App app;
	return appManager.run(app);
}