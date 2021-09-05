#include "App.h"
#include <Cool/App/Input.h>
#include <Cool/Gpu/Vulkan/Context.h>
#include <Cool/Image/ImageSizeConstraintU.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Serialization/JsonFile.h>
#include <Cool/Time/Time.h>

App::App(Window& mainWindow)
    : m_mainWindow(mainWindow)
// , m_shader("Cool/Renderer_Fullscreen/fullscreen.vert", "shaders/demo.frag")
{
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
    ImageSizeConstraintU::apply(_preview_constraint, _render_target);
    ImageSizeConstraintU::apply(_preview_constraint, _render_target2);
    // m_renderer.begin();
    // {
    // 	glClearColor(m_bgColor.r, m_bgColor.g, m_bgColor.b, 1.0f);
    // 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 	m_shader.bind();
    // 	m_renderer.render();
    // }
    // m_renderer.end();
    _render_target.update_render_target_size();
    _render_target2.update_render_target_size();
    render(*_render_target, Time::time());
    render(*_render_target2, -Time::time());
    _exporter.update({*_render_target, [&](RenderTarget& render_target) {
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
    _render_target.imgui_window("1", aspect_ratio_is_constrained);
    _render_target2.imgui_window("2", aspect_ratio_is_constrained);
    _exporter.imgui_window_export_image({*_render_target,
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

void App::onMouseMoveEvent(double xpos, double ypos)
{
    if (!_exporter.is_exporting() && !ImGui::GetIO().WantCaptureMouse) {
    }
}