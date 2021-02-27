#pragma once

#include <Cool/App/IApp.h>
#include <Cool/OpenGL/Shader.h>
#include <Cool/Renderer_Fullscreen/Renderer_Fullscreen.h>
#include <Cool/App/OpenGLWindow.h>

#include "Serialization/SerializedClassExample.h"

using namespace Cool;

class App : public Cool::IApp {
public:
	App(OpenGLWindow& mainWindow);
	~App();

	void update() override;
	void ImGuiWindows() override;
	void ImGuiMenus() override;

	void onKeyboardEvent(int key, int scancode, int action, int mods) override;
	void onMouseButtonEvent(int button, int action, int mods) override;
	void onScrollEvent(double xOffset, double yOffset) override;
	void onMouseMoveEvent(double xPos, double yPos) override;

private:
	OpenGLWindow& m_mainWindow;
	Renderer_Fullscreen m_renderer;
	Shader m_shader;
	glm::vec3 m_bgColor = glm::vec3(0.478f, 0.674f, 0.792f);
	SerializedClassExample m_serializedClassExample;
#ifndef NDEBUG
	bool m_bShow_Debug = true;
	bool m_bShow_ImGuiDemo = false;
#endif

private:
	//Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("A serialization example", m_serializedClassExample),
			CEREAL_NVP(m_bgColor)
		);
	}
};