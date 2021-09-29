#include "App.h"
#include <Cool/Camera/HookEvents.h>
#include <Cool/Gpu/Vulkan/Context.h>
#include <Cool/Image/ImageSizeU.h>
#include <Cool/Input/Input.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Serialization/JsonFile.h>
#include <Cool/Time/Time.h>

App::App(Window& mainWindow)
    : m_mainWindow(mainWindow)
    , _view{_views.make_view("1")}
    , _view2{_views.make_view("2")}
{
    Cool::hook_events(_view2.view.mouse_events(), _camera_controller, _camera);
    Serialization::from_json(*this, File::root_dir() + "/last-session-cache.json");
    Log::ToUser::info(
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
    Serialization::to_json(*this, File::root_dir() + "/last-session-cache.json", "App");
#if defined(__COOL_APP_VULKAN)
    vkDeviceWaitIdle(Vulkan::context().g_Device); // TODO is this necessary ? Is this the right place ?
#endif
}

void App::update()
{
    Time::update();
    for (auto& view : _views) {
        view.update_size(_preview_constraint);
    }
    render(_view.render_target, _fullscreen_pipeline_2D, Time::time());
    render(_view2.render_target, _fullscreen_pipeline_3D, Time::time());
    _exporter.update({_view2.render_target, [&](RenderTarget& render_target) {
                          render(render_target, _fullscreen_pipeline_3D, Time::time());
                      }});
}

void App::render(RenderTarget& render_target, FullscreenPipeline& pipeline, float time)
{
#if defined(__COOL_APP_VULKAN)
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
        ImageSizeU::aspect_ratio(render_target.current_size()),
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

#elif defined(__COOL_APP_OPENGL)
    render_target.render([&]() {
        glClearColor(1., 0., 1., 1.);
        glClear(GL_COLOR_BUFFER_BIT);
        pipeline.shader().bind();
        pipeline.shader().set_uniform("u.time", time);
        pipeline.shader().set_uniform("u.aspect_ratio", ImageSizeU::aspect_ratio(render_target.current_size()));
        pipeline.shader().set_uniform("u.focal_length", 1.f);
        pipeline.shader().set_uniform("u.camera_right_axis", _camera.right_axis());
        pipeline.shader().set_uniform("u.camera_up_axis", _camera.up_axis());
        pipeline.shader().set_uniform("u.camera_front_axis", _camera.front_axis());
        pipeline.shader().set_uniform("u.camera_position", _camera.position());
        pipeline.draw();
    });
#endif
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
    m_serializedClassExample.ImGui();
    ImGui::End();
    Log::ToUser::imgui_console_window();
    //
    ImGui::Begin("Time");
    Time::imgui_timeline();
    ImGui::End();
    //
    bool aspect_ratio_is_constrained = _exporter.is_exporting() || _preview_constraint.wants_to_constrain_aspect_ratio();
    for (auto& view : _views) {
        view.imgui_window(aspect_ratio_is_constrained);
    }
    _exporter.imgui_window_export_image({_view2.render_target,
                                         [&](RenderTarget& render_target) { render(render_target, _fullscreen_pipeline_3D, Time::time()); }});
    _exporter.imgui_window_export_image_sequence();
//
#if defined(DEBUG)
    if (m_bShow_Debug) {
        ImGui::Begin("Debug", &m_bShow_Debug);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        m_mainWindow.imgui_cap_framerate();
        ImGui::Checkbox("Show Demo Window", &m_bShow_ImGuiDemo);
        ImGui::End();
    }
    if (m_bShow_ImGuiDemo) // Show the big demo window (Most of the sample code is
                           // in ImGui::ShowDemoWindow()! You can browse its code
                           // to learn more about Dear ImGui!).
        ImGui::ShowDemoWindow(&m_bShow_ImGuiDemo);
#endif
}

void App::imgui_menus()
{
    if (ImGui::BeginMenu("Preview")) {
        _preview_constraint.imgui();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Windows")) {
        Log::ToUser::imgui_toggle_console();
        for (auto& view : _views) {
            view.view.imgui_open_close_checkbox();
        }
#ifndef NDEBUG
        ImGui::Separator();
        ImGui::Checkbox("Debug", &m_bShow_Debug);
#endif
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Export")) {
        _exporter.imgui_menu_items();
        ImGui::EndMenu();
    }
}

void App::on_keyboard_event(const KeyboardEvent& event)
{
}

void App::on_mouse_button(const MouseButtonEvent<WindowCoordinates>& event)
{
    for (auto& view : _views) {
        view.view.dispatch_mouse_button_event(event, m_mainWindow.glfw());
    }
}

void App::on_mouse_scroll(const MouseScrollEvent<WindowCoordinates>& event)
{
    for (auto& view : _views) {
        view.view.dispatch_mouse_scroll_event(event, m_mainWindow.glfw());
    }
}

void App::on_mouse_move(const MouseMoveEvent<WindowCoordinates>& event)
{
    for (auto& view : _views) {
        view.view.dispatch_mouse_move_event(event, m_mainWindow.glfw());
    }
}