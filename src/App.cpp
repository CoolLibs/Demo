#include "App.h"
#include <Cool/App/Input.h>
#include <Cool/Gpu/Vulkan/Context.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Serialization/JsonFile.h>
#include <Cool/Time/Time.h>

App::App(Window& mainWindow)
    : m_mainWindow(mainWindow)
    , _view("1")
    , _view2("2")
{
    _view.mouse_events().move_event().subscribe([](const auto& event) {
        Log::info("{} {}", event.position.x, event.position.y);
    });
    _view2.mouse_events().move_event().subscribe([](const auto& event) {
        Log::warn("{} {}", event.position.x, event.position.y);
    });
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
    if (_view.size()) {
        _render_target.set_size(_preview_constraint.applied_to(*_view.size()));
    }
    if (_view2.size()) {
        _render_target2.set_size(_preview_constraint.applied_to(*_view2.size()));
    }
    render(_render_target, Time::time());
    render(_render_target2, -Time::time());
    _exporter.update({_render_target, [&](RenderTarget& render_target) {
                          render(render_target, Time::time());
                      }});
}

void App::render(RenderTarget& render_target, float time)
{
#if defined(__COOL_APP_VULKAN)
    render_target.render([&](vk::CommandBuffer& cb) {
        _fullscreen_pipeline.rebuild_for_render_target(render_target.info());
        cb.pushConstants(_fullscreen_pipeline.layout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(time), (const void*)&time);
        _fullscreen_pipeline.draw(cb);
    });

#elif defined(__COOL_APP_OPENGL)
    render_target.render([&]() {
        glClearColor(1., 0., 1., 1.);
        glClear(GL_COLOR_BUFFER_BIT);
        _fullscreen_pipeline.shader().bind();
        _fullscreen_pipeline.shader().set_uniform("u.time", time);
        _fullscreen_pipeline.draw();
    });
#endif
}

void App::ImGuiWindows()
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
    _view.imgui_window(_render_target.imgui_texture_id(), _render_target.current_size(), aspect_ratio_is_constrained);
    _view2.imgui_window(_render_target2.imgui_texture_id(), _render_target2.current_size(), aspect_ratio_is_constrained);
    _exporter.imgui_window_export_image({_render_target,
                                         [&](RenderTarget& render_target) { render(render_target, Time::time()); }});
    _exporter.imgui_window_export_image_sequence();
//
#if defined(DEBUG)
    if (m_bShow_Debug) {
        ImGui::Begin("Debug", &m_bShow_Debug);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        m_mainWindow.imgui_cap_framerate();
        // ImGui::Text("Rendering Size : %d %d", PreviewOptions::Size().width(),
        //             PreviewOptions::Size().height());
        ImGui::Text("Mouse Position in Render Area : %.0f %.0f screen coordinates",
                    Input::MouseInScreenCoordinates().x,
                    Input::MouseInScreenCoordinates().y);
        ImGui::Text("Mouse Position Normalized : %.2f %.2f",
                    Input::MouseInNormalizedRatioSpace().x,
                    Input::MouseInNormalizedRatioSpace().y);
        ImGui::ColorEdit3("Background Color", glm::value_ptr(m_bgColor));
        ImGui::Checkbox("Show Demo Window", &m_bShow_ImGuiDemo);
        ImGui::End();
    }
    if (m_bShow_ImGuiDemo) // Show the big demo window (Most of the sample code is
                           // in ImGui::ShowDemoWindow()! You can browse its code
                           // to learn more about Dear ImGui!).
        ImGui::ShowDemoWindow(&m_bShow_ImGuiDemo);
#endif
}

void App::ImGuiMenus()
{
    if (ImGui::BeginMenu("Preview")) {
        _preview_constraint.imgui();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Windows")) {
        Log::ToUser::imgui_toggle_console();
        _view.imgui_open_close_checkbox();
        _view2.imgui_open_close_checkbox();
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

bool App::should_show_menu_bar()
{
    return !_exporter.is_exporting();
}

void App::onKeyboardEvent(int key, int scancode, int action, int mods)
{
    if (!_exporter.is_exporting() && !ImGui::GetIO().WantTextInput) {
    }
}

void App::onMouseButtonEvent(int button, int action, int mods)
{
    if (!_exporter.is_exporting() && !ImGui::GetIO().WantCaptureMouse) {
    }
}

void App::onScrollEvent(double xOffset, double yOffset)
{
    if (!_exporter.is_exporting() && !ImGui::GetIO().WantCaptureMouse) {
    }
}

void App::on_mouse_move(const MouseMoveEvent<MainWindowCoordinates>& event)
{
    if (!_exporter.is_exporting()) {
        const auto pos = [&]() {
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                int x, y;
                glfwGetWindowPos(m_mainWindow.glfw(), &x, &y);
                return ScreenCoordinates{event.position + glm::vec2{x, y}};
            }
            else {
                return ScreenCoordinates{event.position}; // We trick ImGui because if viewports are disabled, ImGui functions that pretend to return screen coordinates actually return window coordinates (this is a temporary measure because I know that ImGui plans on fixing this)
            }
        }();
        _view.receive_mouse_move_event({pos});
        _view2.receive_mouse_move_event({pos});
    }
}