#include <Cool/Core/run.h>
#include "App.h"

int main()
{
    Cool::run<App>({Cool::WindowConfig{.title = "You can change the window title in main.cpp"}},
                   Cool::InitConfig{.log_pattern = "%^[%e] %n: %v%$"},
                   Cool::AppManagerConfig{.dispatch_keyboard_events_to_imgui = true});
}