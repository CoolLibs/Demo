#include <Cool/Core/run.h>
#include <Cool/Default/DefaultInit.h>
#include "App.h"

int main()
{
    CoolDefault::init();
    Cool::run<App>({Cool::WindowConfig{.title = "You can change the window title in main.cpp"}},
                   Cool::AppManagerConfig{.dispatch_keyboard_events_to_imgui = true});
}