#include <Cool/Core/initialize_and_shutdown.h>
#include "App.h"

int main()
{
    Cool::initialize();
    Cool::run<App>({WindowCreationParams{.name = "You can change the window name in main.cpp"}});
    Cool::shut_down();
}