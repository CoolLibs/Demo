#include "App.h"
#include <Cool/App/Input.h>
#include <Cool/App/RenderState.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Serialization/JsonFile.h>
#include <Cool/Time/Time.h>
#include <Cool/Vulkan/Context.h>

App::App(Window& mainWindow)
    : m_mainWindow(mainWindow)
    // , m_shader("Cool/Renderer_Fullscreen/fullscreen.vert", "shaders/demo.frag")
    , _render_target{500, 500}
{
    Serialization::from_json(*this, File::root_dir() + "/last-session-cache.json");
    RenderState::SubscribeToSizeChanges([]() {
        Log::info(
            "The size of the rendering area has changed. Look, you can "
            "subscribe to this event !");
    });
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
    vkDeviceWaitIdle(Vulkan::context().g_Device);
}

void App::update()
{
    Time::update();
    // m_renderer.begin();
    // {
    // 	glClearColor(m_bgColor.r, m_bgColor.g, m_bgColor.b, 1.0f);
    // 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 	m_shader.bind();
    // 	m_renderer.render();
    // }
    // m_renderer.end();

    _fullscreen_pipeline.rebuild_for_render_target(_render_target.info());
    float time = Time::time();
    _render_target.render([&](vk::CommandBuffer& cb) {
        cb.pushConstants(_fullscreen_pipeline.layout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(time), (const void*)&time);
        _fullscreen_pipeline.draw(cb);
    });
}

void App::render(vk::CommandBuffer cb)
{
    // _fullscreen_pipeline.rebuild_for_render_target(_render_target.info());
    // _fullscreen_pipeline.draw(cb);
}
#include <Cool/Icons/Icons.h>
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
    _render_target.imgui_window();
    //
#if defined(DEBUG)
    if (m_bShow_Debug) {
        ImGui::Begin("Debug", &m_bShow_Debug);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        m_mainWindow.imgui_cap_framerate();
        ImGui::Text("Rendering Size : %d %d", RenderState::Size().width(),
                    RenderState::Size().height());
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
    if (ImGui::BeginMenu("Windows")) {
        Log::ToUser::imgui_toggle_console();
#ifndef NDEBUG
        ImGui::Separator();
        ImGui::Checkbox("Debug", &m_bShow_Debug);
#endif
        ImGui::EndMenu();
    }
}

void App::onKeyboardEvent(int key, int scancode, int action, int mods)
{
    if (!RenderState::IsExporting() && !ImGui::GetIO().WantTextInput) {
    }
}

void App::onMouseButtonEvent(int button, int action, int mods)
{
    if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
    }
}

void App::onScrollEvent(double xOffset, double yOffset)
{
    if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
    }
}

void App::onMouseMoveEvent(double xpos, double ypos)
{
    if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
    }
}