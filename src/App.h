#pragma once

#include <Cool/AppManager/IApp.h>
#include <Cool/Camera/Camera.h>
#include <Cool/Camera/ViewController_Orbital.h>
#include <Cool/Exporter/Exporter.h>
#include <Cool/File/File.h>
#include <Cool/Gpu/FullscreenPipeline.h>
#include <Cool/Image/ImageSizeConstraint.h>
#include <Cool/Time/Clock_Realtime.h>
#include <Cool/View/RenderableViewManager.h>
#include <Cool/View/View.h>
#include <Cool/Window/WindowManager.h>
#include "Serialization/SerializedClassExample.h"

using namespace Cool;

class App : public Cool::IApp {
public:
    explicit App(WindowManager& windows);
    ~App();

    void update() override;
    bool inputs_are_allowed() const override;
    bool wants_to_show_menu_bar() const override;
    void imgui_windows() override;
    void imgui_menus() override;

    void on_keyboard_event(const KeyboardEvent& event) override;
    void on_mouse_button(const MouseButtonEvent<WindowCoordinates>& event) override;
    void on_mouse_scroll(const MouseScrollEvent<WindowCoordinates>& event) override;
    void on_mouse_move(const MouseMoveEvent<WindowCoordinates>& event) override;

private:
    void     render(RenderTarget& render_target, FullscreenPipeline& pipeline, float time);
    Polaroid polaroid_2D();
    Polaroid polaroid_3D();

private:
    SerializedClassExample _serialized_class_example;

    Cool::Exporter               _exporter;
    Cool::ImageSizeConstraint    _preview_constraint;
    Cool::RenderableViewManager  _views; // Must be before the views because it is used to construct them
    Cool::RenderableView&        _view_2D;
    Cool::RenderableView&        _view_3D;
    Cool::FullscreenPipeline     _fullscreen_pipeline_2D{File::root_dir() + "/shaders/demo_2D.frag"};
    Cool::FullscreenPipeline     _fullscreen_pipeline_3D{File::root_dir() + "/shaders/demo_3D.frag"};
    Cool::Camera                 _camera{{5.f, 1.f, 1.f}};
    Cool::ViewController_Orbital _camera_controller;
    Cool::Clock_Realtime         _clock;
#if defined(DEBUG)
    bool _show_imgui_debug = true;
    bool _show_imgui_demo  = false;
#endif

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("A serialization example", _serialized_class_example));
    }
};