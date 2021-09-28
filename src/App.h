#pragma once

#include <Cool/App/IApp.h>
#include <Cool/Camera/Camera.h>
#include <Cool/Camera/ViewController_Orbital.h>
#include <Cool/Exporter/Exporter.h>
#include <Cool/File/File.h>
#include <Cool/Gpu/FullscreenPipeline.h>
#include <Cool/Image/ImageSizeConstraint.h>
#include <Cool/View/RenderableViewManager.h>
#include <Cool/View/View.h>
#include <Cool/Window/Window.h>
#include "Serialization/SerializedClassExample.h"

using namespace Cool;

class App : public Cool::IApp {
public:
    App(Window& mainWindow);
    ~App();

    void update() override;
    bool inputs_are_allowed() const override;
    bool wants_to_show_menu_bar() const override;
    void ImGuiWindows() override;
    void ImGuiMenus() override;

    void onKeyboardEvent(int key, int scancode, int action, int mods) override;
    void on_mouse_button(const MouseButtonEvent<WindowCoordinates>& event) override;
    void on_mouse_scroll(const MouseScrollEvent<WindowCoordinates>& event) override;
    void on_mouse_move(const MouseMoveEvent<WindowCoordinates>& event) override;

private:
    void render(RenderTarget& render_target, FullscreenPipeline& pipeline, float time);

private:
    Window&                m_mainWindow;
    SerializedClassExample m_serializedClassExample;

    Cool::Exporter               _exporter;
    Cool::ImageSizeConstraint    _preview_constraint;
    Cool::RenderableViewManager  _views; // Must be before the views because it is used to construct them
    Cool::RenderableView&        _view;
    Cool::RenderableView&        _view2;
    Cool::FullscreenPipeline     _fullscreen_pipeline_2D{File::root_dir() + "/shaders/demo_2D.frag"};
    Cool::FullscreenPipeline     _fullscreen_pipeline_3D{File::root_dir() + "/shaders/demo_3D.frag"};
    Cool::Camera                 _camera{{5.f, 1.f, 1.f}};
    Cool::ViewController_Orbital _camera_controller;
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
            cereal::make_nvp("A serialization example", m_serializedClassExample));
    }
};