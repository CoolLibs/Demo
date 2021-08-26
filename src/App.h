#pragma once

#include <Cool/App/IApp.h>
#include <Cool/File/File.h>
#include <Cool/Gpu/Shader.h>
#include <Cool/Gpu/Vulkan/FullscreenPipeline.h>
#include <Cool/Gpu/Vulkan/RenderTarget.h>
#include <Cool/Gpu/Vulkan/ShaderModule.h>
#include <Cool/Renderer_Fullscreen/Renderer_Fullscreen.h>
#include <Cool/Window/Window.h>
#include "Serialization/SerializedClassExample.h"

using namespace Cool;

class App : public Cool::IApp {
public:
    App(Window& mainWindow);
    ~App();

    void update() override;
    void ImGuiWindows() override;
    void ImGuiMenus() override;

    void onKeyboardEvent(int key, int scancode, int action, int mods) override;
    void onMouseButtonEvent(int button, int action, int mods) override;
    void onScrollEvent(double xOffset, double yOffset) override;
    void onMouseMoveEvent(double xPos, double yPos) override;

private:
    Window& m_mainWindow;
    //   Renderer_Fullscreen m_renderer;
    //   Shader m_shader;
    glm::vec3              m_bgColor = glm::vec3(0.478f, 0.674f, 0.792f);
    SerializedClassExample m_serializedClassExample;

    Cool::Vulkan::RenderTarget       _render_target;
    Cool::Vulkan::FullscreenPipeline _fullscreen_pipeline{File::root_dir() + "/shaders/demo.frag"};
    Cool::Vulkan::ShaderModule       _vertex_shader{File::root_dir() + "/Cool/res/shaders/fullscreen.vert", Cool::Gpu::ShaderKind::Vertex};
    Cool::Vulkan::ShaderModule       _fragment_shader{File::root_dir() + "/shaders/demo.frag", Cool::Gpu::ShaderKind::Fragment};
#ifndef NDEBUG
    bool m_bShow_Debug     = true;
    bool m_bShow_ImGuiDemo = false;
#endif

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("A serialization example", m_serializedClassExample),
            CEREAL_NVP(m_bgColor));
    }
};