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

    VkRenderPassBeginInfo rp_begin_info    = {};
    rp_begin_info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin_info.renderPass               = _render_target.info().render_pass;
    rp_begin_info.framebuffer              = *_render_target._framebuffer;
    rp_begin_info.renderArea.extent.width  = _render_target.info().viewport.width();
    rp_begin_info.renderArea.extent.height = _render_target.info().viewport.height();
    rp_begin_info.clearValueCount          = 1;
    ImVec4       clear_color               = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    VkClearValue ClearValue;
    ClearValue.color.float32[0] = 1.f;
    ClearValue.color.float32[1] = 1.f;
    ClearValue.color.float32[2] = 0.f;
    ClearValue.color.float32[3] = 1.f;
    rp_begin_info.pClearValues  = &ClearValue;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = Vulkan::context().command_pool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer _cb_;
    if (vkAllocateCommandBuffers(Vulkan::context().g_Device, &allocInfo, &_cb_) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    vk::CommandBuffer          cb{_cb_};
    vk::CommandBufferBeginInfo bi{};
    cb.begin(bi);
    cb.beginRenderPass(rp_begin_info, vk::SubpassContents::eInline);
    _fullscreen_pipeline.draw(cb);
    cb.endRenderPass();

    {
        VkPipelineStageFlags wait_stage = {};
        VkSubmitInfo         info       = {};
        info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount         = 0;
        // info.pWaitSemaphores            = &image_acquired_semaphore;
        info.pWaitDstStageMask  = &wait_stage;
        info.commandBufferCount = 1;
        VkCommandBuffer mcb{cb};
        info.pCommandBuffers      = &mcb;
        info.signalSemaphoreCount = 0;
        // info.pSignalSemaphores          = &render_complete_semaphore;

        // err = vkEndCommandBuffer(fd->CommandBuffer);
        // Vulkan::check_result(err);
        // err = vkQueueSubmit(Vulkan::context().g_Queue, 1, &info, fd->Fence);
        // Vulkan::check_result(err);

        cb.end();
        vkQueueSubmit(Vulkan::context().g_Queue, 1, &info, VkFence{});
    }
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
    ImGui::Begin("MyImage");
    ImGui::Image(_render_target.texture().imgui_texture_id(), {500, 500});
    ImGui::End();
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