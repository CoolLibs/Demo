#include "App.h"
#include <Cool/App/Input.h>
#include <Cool/App/RenderState.h>
#include <Cool/File/File.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Serialization/JsonFile.h>
#include <imgui/backends/imgui_impl_vulkan.h>

// We will use this simple vertex description.
// It has a 2D location (x, y) and a colour (r, g, b)
struct Vertex {
    glm::vec2 pos;
    glm::vec3 colour;
};

static const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

App::App(vku::Framework& vku_framework, Window& mainWindow)
    : _vku_framework{vku_framework}
    , m_mainWindow{mainWindow}
    , _buffer{vku_framework.device(), vku_framework.memprops(), vertices}
    , _pipeline_layout{vku::PipelineLayoutMaker{}.createUnique(_vku_framework.device())}
// , m_shader("Cool/Renderer_Fullscreen/fullscreen.vert", "shaders/demo.frag")
{
    Serialization::from_json(*this,
                             (File::root_dir() + "/last-session-cache.json").c_str());
    // RenderState::SubscribeToSizeChanges([]() {
    //     Log::info(
    //         "The size of the rendering area has changed. Look, you can "
    //         "subscribe to this event !");
    // });
    Log::ToUser::warn(
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
    m_mainWindow.vku().setStaticCommands([this](
                                             vk::CommandBuffer cb, int imageIndex,
                                             vk::RenderPassBeginInfo& rpbi) {
        static auto pipeline = build_pipeline();
        static auto ww       = m_mainWindow.vku().width();
        static auto wh       = m_mainWindow.vku().height();
        if (ww != m_mainWindow.vku().width() || wh != m_mainWindow.vku().height()) {
            ww       = m_mainWindow.vku().width();
            wh       = m_mainWindow.vku().height();
            pipeline = build_pipeline();
        }
        vk::CommandBufferBeginInfo bi{};
        cb.begin(bi);
        cb.beginRenderPass(rpbi, vk::SubpassContents::eInline);
        cb.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
        cb.bindVertexBuffers(0, _buffer.buffer(), vk::DeviceSize(0));
        cb.draw(3, 1, 0, 0);
        auto* data = ImGui::GetDrawData();
        if (data)
            ImGui_ImplVulkan_RenderDrawData(data, cb);
        cb.endRenderPass();
        cb.end();
    });
}

App::~App()
{
    Serialization::to_json(
        *this, (File::root_dir() + "/last-session-cache.json").c_str(), "App");
}

vk::UniquePipeline App::build_pipeline()
{
    // Make a pipeline to use the vertex format and shaders.
    vku::PipelineMaker pm{m_mainWindow.vku().width(), m_mainWindow.vku().height()};
    pm.shader(vk::ShaderStageFlagBits::eVertex, vert_);
    pm.shader(vk::ShaderStageFlagBits::eFragment, frag_);
    pm.vertexBinding(0, (uint32_t)sizeof(Vertex));
    pm.vertexAttribute(0, 0, vk::Format::eR32G32Sfloat,
                       (uint32_t)offsetof(Vertex, pos));
    pm.vertexAttribute(1, 0, vk::Format::eR32G32B32Sfloat,
                       (uint32_t)offsetof(Vertex, colour));

    // Create a pipeline using a renderPass built for our window.
    auto renderPass = m_mainWindow.vku().renderPass();
    auto cache      = _vku_framework.pipelineCache();

    return pm.createUnique(_vku_framework.device(), cache, *_pipeline_layout, renderPass);
}

void App::update()
{
    // m_renderer.begin();
    // {
    // 	glClearColor(m_bgColor.r, m_bgColor.g, m_bgColor.b, 1.0f);
    // 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 	m_shader.bind();
    // 	m_renderer.render();
    // }
    // m_renderer.end();
}

void App::ImGuiWindows()
{
    //
    ImGui::Begin("Serialization");
    m_serializedClassExample.ImGui();
    ImGui::End();
    Log::ToUser::imgui_console_window();
//
#if defined(DEBUG)
    if (m_bShow_Debug) {
        ImGui::Begin("Debug", &m_bShow_Debug);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ImGui::SameLine();
        // bool capFramerate = m_mainWindow.isVSyncEnabled();
        // if (ImGui::Checkbox("Cap framerate", &capFramerate)) {
        //     if (capFramerate)
        //         m_mainWindow.enableVSync();
        //     else
        //         m_mainWindow.disableVSync();
        // }
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