#include "App.h"
#include "Cool/Path/Path.h"
#include "Cool/Path/PathsConfig.h"
//
#include <Cool/Core/run.h> // Must be included last otherwise it slows down compilation because it includes <cereal/archives/json.hpp>

class PathsConfig : public Cool::PathsConfig {
public:
    PathsConfig()
        : Cool::PathsConfig{"Cool-Demo"}
    {}

    [[nodiscard]] auto default_texture() const -> std::filesystem::path override
    {
        return Cool::Path::root() / "res/logo.png";
    }
};

auto main() -> int
{
    Cool::Path::initialize<PathsConfig>();
    Cool::run<Demo::App>({
        .windows_configs   = {Cool::WindowConfig{
              .title                  = "You can change the window title in main.cpp",
              .maximize_on_startup_if = true,
        }},
        .imgui_ini_version = 0,
    });
}