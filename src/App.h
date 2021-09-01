#pragma once

#include <Cool/App/IApp.h>
#include <Cool/Exporter/Exporter.h>
#include <Cool/File/File.h>
#include <Cool/Gpu/FullscreenPipeline.h>
#include <Cool/Gpu/RenderTarget.h>
#include <Cool/Image/ImageSizeConstraint.h>
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
    bool should_show_menu_bar() override;

    void onKeyboardEvent(int key, int scancode, int action, int mods) override;
    void onMouseButtonEvent(int button, int action, int mods) override;
    void onScrollEvent(double xOffset, double yOffset) override;
    void onMouseMoveEvent(double xPos, double yPos) override;

private:
    void render(RenderTarget& render_target, float time);

private:
    Window&                m_mainWindow;
    glm::vec3              m_bgColor = glm::vec3(0.478f, 0.674f, 0.792f);
    SerializedClassExample m_serializedClassExample;

    Cool::Exporter            _exporter;
    Cool::ImageSizeConstraint _preview_constraint;
    Cool::RenderTarget        _render_target;
    Cool::RenderTarget        _render_target2;
    Cool::FullscreenPipeline  _fullscreen_pipeline{File::root_dir() + "/shaders/demo.frag"};
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