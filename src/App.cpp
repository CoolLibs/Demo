#include "App.h"
#include "Cool/DebugOptions/debug_options_windows.h"
#include "Cool/ImGui/icon_fmt.h"
#include "Cool/Log/ToUser.h"

namespace Demo {

App::App(Cool::WindowManager& windows, Cool::ViewsManager& views)
    : _main_window{windows.main_window()}
    , _view{views.make_view<Cool::RenderView>(Cool::ViewCreationParams{
          .name        = Cool::icon_fmt("View", ICOMOON_IMAGE),
          .is_closable = false,
          .start_open  = true,
      })}
{
    // _project.camera_3D_manager.hook_events(_preview_view.mouse_events(), command_executor());
    // _project.camera_2D_manager.hook_events(_preview_view.mouse_events(), command_executor());
}

void App::update()
{
    _clock.update();
    request_rerender();

    if (ImGui::IsKeyPressed(ImGuiKey_Space))
        _clock.toggle_play_pause();
}

void App::render(Cool::RenderTarget& render_target, float time)
{
    if (DebugOptions::log_when_rendering())
        Cool::Log::ToUser::info("App", "Rendered");
    render_target.render([&]() {
        glClearColor(std::sin(time) * 0.5f + 0.5f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        // if (_fullscreen_pipeline.shader().has_value())
        // {
        //     _fullscreen_pipeline.shader()->bind();
        //     _fullscreen_pipeline.shader()->set_uniform("u.time", time);
        //     _fullscreen_pipeline.shader()->set_uniform("u.aspect_ratio", img::SizeU::aspect_ratio(render_target.current_size()));
        //     _fullscreen_pipeline.shader()->set_uniform("u.focal_length", 1.f);
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_right_axis", _camera->right_axis());
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_up_axis", _camera->up_axis());
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_front_axis", _camera->front_axis());
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_position", _camera->position());
        // }
        // _fullscreen_pipeline.draw();
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

    ImGui::Begin("Serialization");
    _serialization_example.imgui();
    ImGui::End();

    DebugOptions::show_framerate_window([&] {
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        _main_window.imgui_cap_framerate();
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
}

} // namespace Demo