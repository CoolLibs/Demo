#pragma once
#include <Cool/AppManager/IApp.h>
#include <Cool/DebugOptions/DebugOptions.h>
#include <Cool/DebugOptions/DebugOptionsManager.h>
#include <Cool/Path/Path.h>
#include <Cool/View/RenderView.h>
#include <Cool/View/ViewsManager.h>
#include <WebGPU/webgpu.hpp>
#include "Cool/Image/ImageSizeConstraint.h"
#include "Cool/Time/Clock_Realtime.h"
#include "Cool/Tips/TipsManager.h"
#include "Cool/WebGPU/FullscreenPipeline.h"
#include "Debug/DebugOptions.h"
#include "examples/SerializationExample.h"

namespace Demo {

using DebugOptionsManager = Cool::DebugOptionsManager<
    DebugOptions,
    Cool::DebugOptions>;

class App : public Cool::IApp { // NOLINT(*special-member-functions)
public:
    explicit App(Cool::ViewsManager& views);
    virtual ~App() = default;

    void update() override;
    void request_rerender() override;
    bool inputs_are_allowed() const override;
    bool wants_to_show_menu_bar() const override;
    void on_shutdown() override;

    void imgui_windows() override;
    void imgui_menus() override;

private:
    void render(Cool::RenderTarget& render_target, float time);

private:
    Cool::FullscreenPipeline  _pipeline;
    Cool::FullscreenPipeline  _pipeline2;
    Cool::RenderView&         _view; // NOLINT(*avoid-const-or-ref-data-members)
    Cool::ImageSizeConstraint _view_constraint{};
    SerializationExample      _serialization_example;
    Cool::TipsManager         _tips_manager{};
    Cool::Clock_Realtime      _clock{};
    // Cool::FullscreenPipeline _fullscreen_pipeline{Cool::File::to_string(Cool::Path::root() + "/shaders/demo_3D.frag"), "demo_3D.frag"};

    // // Must be declared last because its constructor modifies App, and its destructor requires all other members to still be alive
    // Cool::AutoSerializer<App> _auto_serializer{Cool::Path::root() + "/last-session-cache.json", "App", *this};

    DebugOptionsManager::AutoSerializer _auto_serializer_for_debug_options{};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::make_nvp("A serialization example", _serialization_example));
    }
};

} // namespace Demo