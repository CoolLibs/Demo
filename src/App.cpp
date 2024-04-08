#include "App.h"
#include "Cool/Backend/Window.h"
#include "Cool/DebugOptions/debug_options_windows.h"
#include "Cool/Gpu/WebGPUContext.h"
#include "Cool/ImGui/icon_fmt.h"
#include "Cool/Log/ToUser.h"
#include "Cool/Path/Path.h"

namespace Demo {

App::App(Cool::ViewsManager& views)
    : _view{views.make_view<Cool::RenderView>(Cool::ViewCreationParams{
        .name        = Cool::icon_fmt("View", ICOMOON_IMAGE),
        .is_closable = false,
        .start_open  = true,
    })}
    , _pipeline{{
          .label     = "test",
          .wgsl_code = R"wgsl(
struct VertexOutput {
    @location(0) uv: vec2f,
};
@fragment
fn main(in: VertexOutput) -> @location(0) vec4f {
    let d = length(in.uv);
    const margin = 0.2;
    let alpha = smoothstep(1. + margin, 1. - margin, d);
    return vec4(vec3(1.), 1.) * alpha;
}
)wgsl",
      }}
    , _pipeline2{{
          .label     = "test2",
          .wgsl_code = R"wgsl(
struct VertexOutput {
    @location(0) uv: vec2f,
};
@fragment
fn main(in: VertexOutput) -> @location(0) vec4f {
    let d = length(in.uv - vec2(0.5, 0.5));
    const margin = 0.2;
    let alpha = smoothstep(1. + margin, 1. - margin, d);
    return vec4(1.,0., 0., 1.) * alpha;
}
)wgsl",
      }}
{
    // _project.camera_3D_manager.hook_events(_preview_view.mouse_events(), command_executor());
    // _project.camera_2D_manager.hook_events(_preview_view.mouse_events(), command_executor());
}

// TODO(WebGPU) Rename debug option "Log OpenGL info"

void App::update()
{
    _clock.update();
    request_rerender();

    if (ImGui::IsKeyPressed(ImGuiKey_Space))
        _clock.toggle_play_pause();

    _view.update_size(_view_constraint);
}

void App::render(Cool::RenderTarget& render_target, float time)
{
    if (DebugOptions::log_when_rendering())
        Cool::Log::ToUser::info("App", "Rendered");

    render_target.render([&](auto&& render_pass) {
        _pipeline.set_uniforms(img::aspect_ratio(render_target.desired_size()));
        _pipeline.draw(render_pass);
        _pipeline2.set_uniforms(img::aspect_ratio(render_target.desired_size()));
        _pipeline2.draw(render_pass);
    });
}

void App::request_rerender()
{
    render(_view.render_target(), _clock.time_in_seconds());
}

bool App::inputs_are_allowed() const
{
    return true;
}

bool App::wants_to_show_menu_bar() const
{
    return true;
}

void App::on_shutdown()
{
    _tips_manager.on_app_shutdown();
}

void App::imgui_windows()
{
    _view.imgui_window();

    ImGui::Begin("Demo");
    _serialization_example.imgui();
    if (ImGui::Button("Save Image"))
    {
        _view.render_target().texture_straight_alpha().save(Cool::Path::root() / "img_test.png");
    }
    ImGui::End();

    Cool::Log::ToUser::console().imgui_window();
#if DEBUG
    Cool::Log::Debug::console().imgui_window();
#endif

    DebugOptions::show_framerate_window([&] {
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        Cool::window().imgui_cap_framerate();
    });
    if (DebugOptions::show_imgui_demo_window())                         // Show the big demo window (Most of the sample code is
        ImGui::ShowDemoWindow(&DebugOptions::show_imgui_demo_window()); // in ImGui::ShowDemoWindow()! You can browse its code
                                                                        // to learn more about Dear ImGui!).

    Cool::debug_options_windows(_tips_manager);
}

void App::imgui_menus()
{
    static bool was_closed_last_frame{true}; // HACK: I guess a `static` here is okay because no one is gonna want two distinct instances of the same debug menu O:) A better solution would be to make a small Menu class that would remember if it was open last frame or not.
    if (ImGui::BeginMenu("Debug"))
    {
        DebugOptionsManager::imgui_ui_for_all_options(was_closed_last_frame);
        was_closed_last_frame = false;
        ImGui::EndMenu();
    }
    else
    {
        was_closed_last_frame = true;
    }
    if (ImGui::BeginMenu(Cool::icon_fmt("View", ICOMOON_IMAGE, true).c_str()))
    {
        _view_constraint.imgui();
        ImGui::EndMenu();
    }
}

} // namespace Demo