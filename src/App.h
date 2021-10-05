#pragma once

#include <Cool/AppManager/IApp.h>
#include <Cool/Camera/Camera.h>
#include <Cool/Camera/ViewController_Orbital.h>
#include <Cool/Exporter/Exporter.h>
#include <Cool/File/File.h>
#include <Cool/Gpu/FullscreenPipeline.h>
#include <Cool/Image/ImageSizeConstraint.h>
#include <Cool/Serialization/AutoSerializer.h>
#include <Cool/Time/Clock_Realtime.h>
#include <Cool/View/RenderableViewManager.h>
#include <Cool/View/View.h>
#include <Cool/Window/WindowManager.h>
#include "examples/SerializationExample.h"

class App : public Cool::IApp {
public:
    explicit App(Cool::WindowManager& windows);

    void update() override;
    bool inputs_are_allowed() const override;
    bool wants_to_show_menu_bar() const override;
    void imgui_windows() override;
    void imgui_menus() override;

    void on_keyboard_event(const Cool::KeyboardEvent& event) override;
    void on_mouse_button(const Cool::MouseButtonEvent<Cool::WindowCoordinates>& event) override;
    void on_mouse_scroll(const Cool::MouseScrollEvent<Cool::WindowCoordinates>& event) override;
    void on_mouse_move(const Cool::MouseMoveEvent<Cool::WindowCoordinates>& event) override;

private:
    void           render(Cool::RenderTarget& render_target, Cool::FullscreenPipeline& pipeline, float time);
    Cool::Polaroid polaroid_2D();
    Cool::Polaroid polaroid_3D();

private:
    SerializationExample _serialization_example;

    Cool::Window&                _main_window;
    Cool::Exporter               _exporter;
    Cool::ImageSizeConstraint    _preview_constraint;
    Cool::RenderableViewManager  _views; // Must be before the views because it is used to construct them
    Cool::RenderableView&        _view_2D;
    Cool::RenderableView&        _view_3D;
    Cool::FullscreenPipeline     _fullscreen_pipeline_2D{Cool::File::root_dir() + "/shaders/demo_2D.frag"};
    Cool::FullscreenPipeline     _fullscreen_pipeline_3D{Cool::File::root_dir() + "/shaders/demo_3D.frag"};
    Cool::Camera                 _camera{{5.f, 1.f, 1.f}};
    Cool::ViewController_Orbital _camera_controller;
    Cool::Clock_Realtime         _clock;
#if defined(DEBUG)
    bool _show_imgui_debug = true;
    bool _show_imgui_demo  = false;
#endif
    // Must be declared last because its constructor modifies App, and its destructor requires all other members to still be alive
    Cool::AutoSerializer<App> _auto_serializer{Cool::File::root_dir() + "/last-session-cache.json", "App", *this};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("A serialization example", _serialization_example));
    }
};