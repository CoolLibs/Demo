#ifdef DEBUG
    #define DOCTEST_CONFIG_IMPLEMENT
    #include <doctest/doctest.h>
#endif

// #include <Cool/App/AppManager.h>
// #include <Cool/File/File.h>
// #include <Cool/Log/Log.h>
// #include <Cool/Time/Time.h>
// #include "App.h"

#include <Cool/App/WindowFactory.h>

int main(int argc, char** argv)
{
#if defined(DEBUG)
    // Run the tests !
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();
    if (context.shouldExit()) {
        return res;
    }
#endif

    Cool::WindowFactory_Vulkan::initialize();
    Cool::WindowFactory_Vulkan window_factory{};
    Cool::Window               my_window = window_factory.make_window(
        "You can change the window title in main.cpp",
        1280,
        720);

    auto& fw = window_factory.vku_framework();

    // Get a device from the demo framework.
    vk::Device device = fw.device();

    // Create a window to draw into
    vku::Window window{fw.instance(), device, fw.physicalDevice(), fw.graphicsQueueFamilyIndex(), my_window.glfw_window()};
    if (!window.ok()) {
        std::cout << "Window creation failed" << std::endl;
        exit(1);
    }

    // Create two shaders, vertex and fragment. See the files helloTriangle.vert
    // and helloTriangle.frag for details.
    vku::ShaderModule vert_{device, "C:/Dev/Cool/Cool-Demo/Cool/lib/Vookoo/examples/build/helloTriangle.vert.spv"};
    vku::ShaderModule frag_{device, "C:/Dev/Cool/Cool-Demo/Cool/lib/Vookoo/examples/build/helloTriangle.frag.spv"};

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
    vku::HostVertexBuffer buffer(device, fw.memprops(), vertices);

    auto buildPipeline = [&]() {
        // Make a pipeline to use the vertex format and shaders.
        vku::PipelineMaker pm{window.width(), window.height()};
        pm.shader(vk::ShaderStageFlagBits::eVertex, vert_);
        pm.shader(vk::ShaderStageFlagBits::eFragment, frag_);
        pm.vertexBinding(0, (uint32_t)sizeof(Vertex));
        pm.vertexAttribute(0, 0, vk::Format::eR32G32Sfloat,
                           (uint32_t)offsetof(Vertex, pos));
        pm.vertexAttribute(1, 0, vk::Format::eR32G32B32Sfloat,
                           (uint32_t)offsetof(Vertex, colour));

        // Create a pipeline using a renderPass built for our window.
        auto renderPass = window.renderPass();
        auto cache      = fw.pipelineCache();

        return pm.createUnique(device, cache, *pipelineLayout_, renderPass);
    };
    auto pipeline = buildPipeline();

    // We only need to create the command buffer(s) once.
    // This simple function lets us do that.
    window.setStaticCommands([&pipeline, &buffer, &window, &buildPipeline](
                                 vk::CommandBuffer cb, int imageIndex,
                                 vk::RenderPassBeginInfo& rpbi) {
        static auto ww = window.width();
        static auto wh = window.height();
        if (ww != window.width() || wh != window.height()) {
            ww       = window.width();
            wh       = window.height();
            pipeline = buildPipeline();
        }
        vk::CommandBufferBeginInfo bi{};
        cb.begin(bi);
        cb.beginRenderPass(rpbi, vk::SubpassContents::eInline);
        cb.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
        cb.bindVertexBuffers(0, buffer.buffer(), vk::DeviceSize(0));
        cb.draw(3, 1, 0, 0);
        cb.endRenderPass();
        cb.end();
    });

    // Loop waiting for the window to close.
    while (!glfwWindowShouldClose(my_window.glfw_window())) {
        glfwPollEvents();

        // draw one triangle.
        window.draw(device, fw.graphicsQueue());

        // Very crude method to prevent your GPU from overheating.
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    // Wait until all drawing is done and then kill the window.
    //     //     device.waitIdle();
    //     // }
    //     // glfwDestroyWindow(glfwwindow);
    //     // glfwTerminate();
    // Init
    //     Cool::Log::initialize();
    // #ifdef DEBUG
    //     // When launching from an IDE (which almost always means we are in Debug mode)
    //     // the initial current_path will be set to bin/Debug
    //     // but I prefer to have it at the root of the project, so that the assets we load
    //     // are the ones at the root of the project, not the ones that have been copied to the bin folders.
    //     // This means that if for example we do hot reloading of shaders
    //     // we can modify the source assets (at the root), instead of the ones that have been copied.
    //     // This is important because only the ones at the root are tracked in Git.
    //     // Also, if you modify an asset at the root whithout modifying some code, and then restart the program
    //     // the asset won't be copied (because I couldn't figure out how to configure CMake to do so)
    //     // So it is better to always work with the assets at the root while in development.
    //     std::filesystem::current_path(CMAKE_SOURCE_DIR);
    // #endif
    //     Cool::File::initialize_root_dir(std::filesystem::current_path().string());
    //     Cool::Time::initialize();
    //     // Init Glfw and set OpenGL version
    // #ifndef __APPLE__
    //     Cool::WindowFactory window_factory(4, 3);
    // #else
    //     Cool::WindowFactory window_factory(3, 3); // OpenGL > 3.3 is not supported on MacOS
    // #endif
    //     // Create the main window and init OpenGL
    //     Cool::Window& mainWindow = window_factory.create("You can change the window name in main.cpp", 1280, 720);
    // #ifndef DEBUG
    //     glfwMaximizeWindow(mainWindow.get());
    // #endif
    //     // App
    //     App              app(mainWindow);
    //     Cool::AppManager appManager(mainWindow, app);
    //     appManager.run();
}