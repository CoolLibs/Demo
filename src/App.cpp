#include "App.h"
#include "Cool/Backend/Window.h"
#include "Cool/DebugOptions/debug_options_windows.h"
#include "Cool/Gpu/WebGPUContext.h"
#include "Cool/ImGui/icon_fmt.h"
#include "Cool/Log/ToUser.h"
#include "Cool/WebGPU/ShaderModule.h"

namespace Demo {

App::App(Cool::ViewsManager& views)
    : _view{views.make_view<Cool::RenderView>(Cool::ViewCreationParams{
        .name        = Cool::icon_fmt("View", ICOMOON_IMAGE),
        .is_closable = false,
        .start_open  = true,
    })}
{
    Cool::ShaderModule shader_module{R"wgsl(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> {
	var p = vec2f(0.0, 0.0);
	if (in_vertex_index == 0u) {
		p = vec2f(-0.5, -0.5);
	} else if (in_vertex_index == 1u) {
		p = vec2f(0.5, -0.5);
	} else {
		p = vec2f(0.0, 0.5);
	}
	return vec4f(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(0.0, 0.4, 1.0, 1.0);
}
)wgsl"};
    std::cout << "Creating render pipeline..." << std::endl;
    wgpu::RenderPipelineDescriptor pipelineDesc;

    // Vertex fetch
    // (We don't use any input buffer so far)
    pipelineDesc.vertex.bufferCount = 0;
    pipelineDesc.vertex.buffers     = nullptr;

    // Vertex shader
    pipelineDesc.vertex.module        = shader_module.handle();
    pipelineDesc.vertex.entryPoint    = "vs_main";
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants     = nullptr;

    // Primitive assembly and rasterization
    // Each sequence of 3 vertices is considered as a triangle
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    // We'll see later how to specify the order in which vertices should be
    // connected. When not specified, vertices are considered sequentially.
    pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    // The face orientation is defined by assuming that when looking
    // from the front of the face, its corner vertices are enumerated
    // in the counter-clockwise (CCW) order.
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    // But the face orientation does not matter much because we do not
    // cull (i.e. "hide") the faces pointing away from us (which is often
    // used for optimization).
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

    // Fragment shader
    wgpu::FragmentState fragmentState;
    pipelineDesc.fragment       = &fragmentState;
    fragmentState.module        = shader_module.handle();
    fragmentState.entryPoint    = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants     = nullptr;

    // Configure blend state
    wgpu::BlendState blendState;
    // Usual alpha blending for the color:
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    // We leave the target alpha untouched:
    blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
    blendState.alpha.dstFactor = wgpu::BlendFactor::One;
    blendState.alpha.operation = wgpu::BlendOperation::Add;

    wgpu::ColorTargetState colorTarget;
    colorTarget.format    = Cool::webgpu_context().swapChainFormat;
    colorTarget.blend     = &blendState;
    colorTarget.writeMask = wgpu::ColorWriteMask::All; // We could write to only some of the color channels.

    // We have only one target because our render pass has only one output color
    // attachment.
    fragmentState.targetCount = 1;
    fragmentState.targets     = &colorTarget;

    // We setup a depth buffer state for the render pipeline
    wgpu::DepthStencilState depthStencilState = wgpu::Default;
    // Keep a fragment only if its depth is lower than the previously blended one
    depthStencilState.depthCompare = wgpu::CompareFunction::Less;
    // Each time a fragment is blended into the target, we update the value of the Z-buffer
    depthStencilState.depthWriteEnabled = true;
    // Store the format in a variable as later parts of the code depend on it
    wgpu::TextureFormat depthTextureFormat = wgpu::TextureFormat::Depth24Plus;
    depthStencilState.format               = depthTextureFormat;
    // Deactivate the stencil alltogether
    depthStencilState.stencilReadMask  = 0;
    depthStencilState.stencilWriteMask = 0;

    pipelineDesc.depthStencil = &depthStencilState;

    // Multi-sampling
    // Samples per pixel
    pipelineDesc.multisample.count = 1;
    // Default value for the mask, meaning "all bits on"
    pipelineDesc.multisample.mask = ~0u;
    // Default value as well (irrelevant for count = 1 anyways)
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    // Pipeline layout
    pipelineDesc.layout = nullptr;

    _pipeline = Cool::webgpu_context().device.createRenderPipeline(pipelineDesc);
    std::cout << "Render pipeline: " << *_pipeline << std::endl;
    // _project.camera_3D_manager.hook_events(_preview_view.mouse_events(), command_executor());
    // _project.camera_2D_manager.hook_events(_preview_view.mouse_events(), command_executor());
}

void App::update()
{
    _clock.update();
    request_rerender();

    if (ImGui::IsKeyPressed(ImGuiKey_Space))
        _clock.toggle_play_pause();

    // In its overall outline, drawing a triangle is as simple as this:
    // Select which render pipeline to use
    Cool::webgpu_context().mainRenderPass.setPipeline(*_pipeline);
    // Draw 1 instance of a 3-vertices shape
    Cool::webgpu_context().mainRenderPass.draw(3, 1, 0, 0);
}

void App::render(Cool::RenderTarget& render_target, float time)
{
    if (DebugOptions::log_when_rendering())
        Cool::Log::ToUser::info("App", "Rendered");
    render_target.render([&]() {
        // glClearColor(std::sin(time) * 0.5f + 0.5f, 0.f, 1.f, 1.f);
        // glClear(GL_COLOR_BUFFER_BIT);

        // if (_fullscreen_pipeline.shader().has_value())
        // {
        //     _fullscreen_pipeline.shader()->bind();
        //     _fullscreen_pipeline.shader()->set_uniform("u.time", time);
        //     _fullscreen_pipeline.shader()->set_uniform("u.aspect_ratio", img::SizeU::aspect_ratio(render_target.current_size()));
        //     _fullscreen_pipeline.shader()->set_uniform("u.focal_length", 1.f);
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_right_axis", _camera->right_axis());
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_up_axis", _camera->up_axis());
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_front_axis", _camera->front_axis());
        //     _fullscreen_pipeline.shader()->set_uniform("u.camera_position", _camera->position());
        // }
        // _fullscreen_pipeline.draw();
    });
}

void App::request_rerender()
{
    render(_view.render_target(), _clock.time_in_seconds());
}

bool App::inputs_are_allowed() const
{
    return true;
}

bool App::wants_to_show_menu_bar() const
{
    return true;
}

void App::on_shutdown()
{
    _tips_manager.on_app_shutdown();
}

void App::imgui_windows()
{
    _view.imgui_window();

    ImGui::Begin("Serialization");
    _serialization_example.imgui();
    ImGui::End();

    DebugOptions::show_framerate_window([&] {
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        Cool::window().imgui_cap_framerate();
    });
    if (DebugOptions::show_imgui_demo_window())                         // Show the big demo window (Most of the sample code is
        ImGui::ShowDemoWindow(&DebugOptions::show_imgui_demo_window()); // in ImGui::ShowDemoWindow()! You can browse its code
                                                                        // to learn more about Dear ImGui!).

    Cool::debug_options_windows(_tips_manager);
}

void App::imgui_menus()
{
    static bool was_closed_last_frame{true}; // HACK: I guess a `static` here is okay because no one is gonna want two distinct instances of the same debug menu O:) A better solution would be to make a small Menu class that would remember if it was open last frame or not.
    if (ImGui::BeginMenu("Debug"))
    {
        DebugOptionsManager::imgui_ui_for_all_options(was_closed_last_frame);
        was_closed_last_frame = false;
        ImGui::EndMenu();
    }
    else
    {
        was_closed_last_frame = true;
    }
}

} // namespace Demo