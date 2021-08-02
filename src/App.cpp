#include "App.h"
#include <Cool/App/Input.h>
#include <Cool/App/RenderState.h>
#include <Cool/File/File.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Serialization/JsonFile.h>
#include <vku/vku.hpp>

App::App(Window& mainWindow)
    : m_mainWindow(mainWindow)
// , m_shader("Cool/Renderer_Fullscreen/fullscreen.vert", "shaders/demo.frag")
{
    Serialization::from_json(*this,
                             (File::root_dir() + "/last-session-cache.json").c_str());
    RenderState::SubscribeToSizeChanges([]() {
        Log::info(
            "The size of the rendering area has changed. Look, you can "
            "subscribe to this event !");
    });
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
}

App::~App()
{
    Serialization::to_json(
        *this, (File::root_dir() + "/last-session-cache.json").c_str(), "App");
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

void App::render(vk::CommandBuffer cb)
{
    auto&             device = m_mainWindow._vulkan_context.g_Device;
    vku::ShaderModule vert_{device, "C:\\Dev\\Cool\\Cool-Demo\\Cool\\lib\\Vookoo\\build\\examples\\helloTriangle.vert.spv"};
    vku::ShaderModule frag_{device, "C:\\Dev\\Cool\\Cool-Demo\\Cool\\lib\\Vookoo\\build\\examples\\helloTriangle.frag.spv"};

    // Make a default pipeline layout. This shows how pointers
    // to resources are layed out.
    vku::PipelineLayoutMaker plm{};
    auto                     pipelineLayout_ = plm.createUnique(device);

    // We will use this simple vertex description.
    // It has a 2D location (x, y) and a colour (r, g, b)
    struct Vertex {
        glm::vec2 pos;
        glm::vec3 colour;
    };

    // This is our triangle.
    const std::vector<Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
    vku::HostVertexBuffer buffer(device, m_mainWindow._vulkan_context.memory_properties, vertices);

    auto buildPipeline = [&, this]() {
        // Make a pipeline to use the vertex format and shaders.
        vku::PipelineMaker pm{
            static_cast<uint32_t>(RenderState::Size().width()),
            static_cast<uint32_t>(RenderState::Size().height())};
        pm.shader(vk::ShaderStageFlagBits::eVertex, vert_);
        pm.shader(vk::ShaderStageFlagBits::eFragment, frag_);
        pm.vertexBinding(0, (uint32_t)sizeof(Vertex));
        pm.vertexAttribute(0, 0, vk::Format::eR32G32Sfloat,
                           (uint32_t)offsetof(Vertex, pos));
        pm.vertexAttribute(1, 0, vk::Format::eR32G32B32Sfloat,
                           (uint32_t)offsetof(Vertex, colour));

        // Create a pipeline using a renderPass built for our window.
        auto renderPass = m_mainWindow._vulkan_window_state.g_MainWindowData.RenderPass;
        auto cache      = m_mainWindow._vulkan_context.g_PipelineCache;

        return pm.createUnique(device, cache, *pipelineLayout_, renderPass);
    };
    auto pipeline = buildPipeline();

    // cb.beginRenderPass(rpbi, vk::SubpassContents::eInline);
    cb.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
    cb.bindVertexBuffers(0, buffer.buffer(), vk::DeviceSize(0));
    cb.draw(3, 1, 0, 0);
    // cb.endRenderPass();
    // cb.end();
}

void App::ImGuiWindows()
{
    //
    ImGui::Begin("Serialization");
    m_serializedClassExample.ImGui();
    ImGui::End();
    Log::ToUser::imgui_console_window();
    //
#ifndef NDEBUG
    if (m_bShow_Debug) {
        ImGui::Begin("Debug", &m_bShow_Debug);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ImGui::SameLine();
        bool capFramerate = m_mainWindow.isVSyncEnabled();
        if (ImGui::Checkbox("Cap framerate", &capFramerate)) {
            if (capFramerate)
                m_mainWindow.enableVSync();
            else
                m_mainWindow.disableVSync();
        }
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