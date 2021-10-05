#include "App.h"
#include <Cool/Camera/HookEvents.h>
#include <Cool/Gpu/Vulkan/Context.h>
#include <Cool/Image/ImageSizeU.h>
#include <Cool/Input/Input.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Serialization/JsonFile.h>
#include <Cool/Time/ClockU.h>

App::App(Cool::WindowManager& windows)
    : _main_window{windows.main_window()}
    , _view_2D{_views.make_view("2D")}
    , _view_3D{_views.make_view("3D")}
{
    Cool::hook_events(_view_3D.view.mouse_events(), _camera_controller, _camera);
    Cool::Serialization::from_json(*this, Cool::File::root_dir() + "/last-session-cache.json");
    Cool::Log::ToUser::info(
        "App::App",
        "You can display messages to the user using Log::ToUser, "
        "and you can {} them !",
        "format");
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Please note that the
    // blending is WRONG for the alpha channel (but it doesn't matter in most
    // cases) The correct call would be glBlendFuncSeparate(GL_SRC_ALPHA,
    // GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE) a.k.a. newAlpha =
    // srcAlpha + dstAlpha - srcAlpha*dstAlpha
}

App::~App()
{
    Cool::Serialization::to_json(*this, Cool::File::root_dir() + "/last-session-cache.json", "App");
}

void App::update()
{
    if (!_exporter.is_exporting()) {
        _clock.update();
        for (auto& view : _views) {
            view.update_size(_preview_constraint);
        }
        render(_view_2D.render_target, _fullscreen_pipeline_2D, _clock.time());
        render(_view_3D.render_target, _fullscreen_pipeline_3D, _clock.time());
    }
    else {
        _exporter.update(polaroid_2D());
    }
}

void App::render(Cool::RenderTarget& render_target, Cool::FullscreenPipeline& pipeline, float time)
{
#if defined(COOL_VULKAN)
    struct alignas(32) PushConstants {
        float     time;
        float     aspect_ratio;
        float     focal_length;
        float     padd_;
        glm::vec3 right;
        float     padd_1;
        glm::vec3 up;
        float     padd_2;
        glm::vec3 front;
        float     padd_3;
        glm::vec3 pos;
        float     padd_4;
    };
    auto pc = PushConstants{
        time,
        Cool::ImageSizeU::aspect_ratio(render_target.current_size()),
        1.f,
        0.1,
        _camera.right_axis(),
        0.1,
        _camera.up_axis(),
        0.1,
        _camera.front_axis(),
        0.1,
        _camera.position(),
        0.1};
    render_target.render([&](vk::CommandBuffer& cb) {
        pipeline.rebuild_for_render_target(render_target.info());
        cb.pushConstants(pipeline.layout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(pc), (const void*)&pc);
        pipeline.draw(cb);
    });

#elif defined(COOL_OPENGL)
    render_target.render([&]() {
        glClearColor(1., 0., 1., 1.);
        glClear(GL_COLOR_BUFFER_BIT);
        pipeline.shader().bind();
        pipeline.shader().set_uniform("u.time", time);
        pipeline.shader().set_uniform("u.aspect_ratio", Cool::ImageSizeU::aspect_ratio(render_target.current_size()));
        pipeline.shader().set_uniform("u.focal_length", 1.f);
        pipeline.shader().set_uniform("u.camera_right_axis", _camera.right_axis());
        pipeline.shader().set_uniform("u.camera_up_axis", _camera.up_axis());
        pipeline.shader().set_uniform("u.camera_front_axis", _camera.front_axis());
        pipeline.shader().set_uniform("u.camera_position", _camera.position());
        pipeline.draw();
    });
#endif
}

Cool::Polaroid App::polaroid_2D()
{
    return {
        .render_target = _view_2D.render_target,
        .render_fn     = [&](Cool::RenderTarget& render_target, float time) {
            render(render_target, _fullscreen_pipeline_2D, time);
        }};
}

Cool::Polaroid App::polaroid_3D()
{
    return {
        .render_target = _view_3D.render_target,
        .render_fn     = [&](Cool::RenderTarget& render_target, float time) {
            render(render_target, _fullscreen_pipeline_3D, time);
        }};
}

bool App::inputs_are_allowed() const
{
    return !_exporter.is_exporting();
}

bool App::wants_to_show_menu_bar() const
{
    return !_exporter.is_exporting();
}

void App::imgui_windows()
{
    //
    ImGui::Begin("Serialization");
    _serialization_example.imgui();
    ImGui::End();
    Cool::Log::ToUser::imgui_console_window();
    //
    ImGui::Begin("Time");
    Cool::ClockU::imgui_timeline(_clock);
    ImGui::End();
    //
    for (const bool aspect_ratio_is_constrained = _exporter.is_exporting() || // cppcheck-suppress syntaxError // (CppCheck is not yet aware of this C++20 syntax)
                                                  _preview_constraint.wants_to_constrain_aspect_ratio();
         auto& view : _views) {
        view.imgui_window(aspect_ratio_is_constrained);
    }
    _exporter.imgui_windows(polaroid_3D(), _clock.time());
//
#if defined(DEBUG)
    if (_show_imgui_debug) {
        ImGui::Begin("Debug", &_show_imgui_debug);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        _main_window.imgui_cap_framerate();
        ImGui::Checkbox("Show Demo Window", &_show_imgui_demo);
        ImGui::End();
    }
    if (_show_imgui_demo) { // Show the big demo window (Most of the sample code is
                            // in ImGui::ShowDemoWindow()! You can browse its code
                            // to learn more about Dear ImGui!).
        ImGui::ShowDemoWindow(&_show_imgui_demo);
    }
#endif
}

void App::imgui_menus()
{
    if (ImGui::BeginMenu("Preview")) {
        _preview_constraint.imgui();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Windows")) {
        Cool::Log::ToUser::imgui_toggle_console();
        for (auto& view : _views) {
            view.view.imgui_open_close_checkbox();
        }
#if defined(DEBUG)
        ImGui::Separator();
        ImGui::Checkbox("Debug", &_show_imgui_debug);
#endif
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Export")) {
        _exporter.imgui_menu_items();
        ImGui::EndMenu();
    }
}

void App::on_keyboard_event(const Cool::KeyboardEvent& event)
{
}

void App::on_mouse_button(const Cool::MouseButtonEvent<Cool::WindowCoordinates>& event)
{
    for (auto& view : _views) {
        view.view.dispatch_mouse_button_event(event, _main_window.glfw());
    }
}

void App::on_mouse_scroll(const Cool::MouseScrollEvent<Cool::WindowCoordinates>& event)
{
    for (auto& view : _views) {
        view.view.dispatch_mouse_scroll_event(event, _main_window.glfw());
    }
}

void App::on_mouse_move(const Cool::MouseMoveEvent<Cool::WindowCoordinates>& event)
{
    for (auto& view : _views) {
        view.view.dispatch_mouse_move_event(event, _main_window.glfw());
    }
}