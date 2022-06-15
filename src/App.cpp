#include "App.h"
#include <Cool/Camera/hook_events.h>
#include <Cool/Gpu/Vulkan/Context.h>
#include <Cool/Image/ImageSizeU.h>
#include <Cool/Input/Input.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Time/ClockU.h>

App::App(Cool::WindowManager& windows)
    : DefaultApp::DefaultApp{windows, [&](Cool::RenderTarget& render_target, float time) {
                                 render(render_target, time);
                             }}
{
    Cool::Log::ToUser::info("App::App",
                            "You can display messages to the user using Log::ToUser, and you can {} them !",
                            "format");
}

void App::update()
{
    DefaultApp::update();
    if (inputs_are_allowed())
    {
    }
}

void App::render(Cool::RenderTarget& render_target, float time)
{
#if defined(COOL_VULKAN)
    struct alignas(32) PushConstants {
        float     time;
        float     aspect_ratio;
        float     focal_length;
        float     padd_;
        glm::vec3 right;
        float     padd_1;
        glm::vec3 up;
        float     padd_2;
        glm::vec3 front;
        float     padd_3;
        glm::vec3 pos;
        float     padd_4;
    };
    auto pc = PushConstants{
        time,
        Cool::ImageSizeU::aspect_ratio(render_target.current_size()),
        1.f,
        0.1f,
        _camera->right_axis(),
        0.1f,
        _camera->up_axis(),
        0.1f,
        _camera->front_axis(),
        0.1f,
        _camera->position(),
        0.1f};
    render_target.render([&](vk::CommandBuffer& cb) {
        _fullscreen_pipeline.rebuild_for_render_target(render_target.info());
        cb.pushConstants(_fullscreen_pipeline.layout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(pc), (const void*)&pc);
        _fullscreen_pipeline.draw(cb);
    });

#elif defined(COOL_OPENGL)
    render_target.render([&]() {
        glClearColor(1., 0., 1., 1.);
        glClear(GL_COLOR_BUFFER_BIT);
        if (_fullscreen_pipeline.shader().has_value())
        {
            _fullscreen_pipeline.shader()->bind();
            _fullscreen_pipeline.shader()->set_uniform("u.time", time);
            _fullscreen_pipeline.shader()->set_uniform("u.aspect_ratio", img::SizeU::aspect_ratio(render_target.current_size()));
            _fullscreen_pipeline.shader()->set_uniform("u.focal_length", 1.f);
            _fullscreen_pipeline.shader()->set_uniform("u.camera_right_axis", _camera->right_axis());
            _fullscreen_pipeline.shader()->set_uniform("u.camera_up_axis", _camera->up_axis());
            _fullscreen_pipeline.shader()->set_uniform("u.camera_front_axis", _camera->front_axis());
            _fullscreen_pipeline.shader()->set_uniform("u.camera_position", _camera->position());
        }
        _fullscreen_pipeline.draw();
    });
#endif
}

void App::imgui_windows()
{
    DefaultApp::imgui_windows();
    if (inputs_are_allowed())
    {
        ImGui::Begin("Serialization");
        _serialization_example.imgui();
        ImGui::End();
    }
}

void App::imgui_menus()
{
    DefaultApp::imgui_menus();
}

void App::on_keyboard_event(const Cool::KeyboardEvent& event)
{
    DefaultApp::on_keyboard_event(event);
}

void App::on_mouse_button(const Cool::MouseButtonEvent<Cool::WindowCoordinates>& event)
{
    DefaultApp::on_mouse_button(event);
}

void App::on_mouse_scroll(const Cool::MouseScrollEvent<Cool::WindowCoordinates>& event)
{
    DefaultApp::on_mouse_scroll(event);
}

void App::on_mouse_move(const Cool::MouseMoveEvent<Cool::WindowCoordinates>& event)
{
    DefaultApp::on_mouse_move(event);
}