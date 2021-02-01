#include "App.h"

#include <Cool/App/RenderState.h>
#include <Cool/App/Input.h>

App::App()
	: m_shader("Cool/Renderer_Fullscreen/fullscreen.vert", "shaders/demo.frag")
{
	RenderState::SubscribeToSizeChanges([]() { Log::Info("The size of the rendering area has changed. Look, you can subscribe to this event !"); });
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Please note that the blending is WRONG for the alpha channel (but it doesn't matter in most cases) The correct call would be glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE) a.k.a. newAlpha = srcAlpha + dstAlpha - srcAlpha*dstAlpha
}

void App::update() {
	m_renderer.begin();
	{
		glClearColor(m_bgColor.r, m_bgColor.g, m_bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_shader.bind();
		m_renderer.render();
	}
	m_renderer.end();
}

void App::ImGuiWindows() {
#ifndef NDEBUG
	if (m_bShow_Debug) {
		ImGui::Begin("Debug", &m_bShow_Debug);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::SameLine();
		static bool capFramerate = true;
		if (ImGui::Checkbox("Cap framerate", &capFramerate)) {
			if (capFramerate)
				SDL_GL_SetSwapInterval(1);
			else
				SDL_GL_SetSwapInterval(0);
		}
		ImGui::Text("Rendering Size : %d %d", RenderState::Size().width(), RenderState::Size().height());
		ImGui::Text("Mouse Position in Render Area : %d %d pixels", Input::MouseInPixels().x, Input::MouseInPixels().y);
		ImGui::Text("Mouse Position in Render Area : %.1f %.1f centimeters", Input::MouseInCentimeters().x, Input::MouseInCentimeters().y);
		ImGui::Text("Mouse Position Normalized : %.2f %.2f", Input::MouseInNormalizedRatioSpace().x, Input::MouseInNormalizedRatioSpace().y);
		ImGui::ColorEdit3("Background Color", glm::value_ptr(m_bgColor));
		ImGui::Checkbox("Show Demo Window", &m_bShow_ImGuiDemo);
		ImGui::End();
	}
	if (m_bShow_ImGuiDemo) // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		ImGui::ShowDemoWindow(&m_bShow_ImGuiDemo);
#endif
}

void App::ImGuiMenus() {
	if (ImGui::BeginMenu("Windows")) {
#ifndef NDEBUG
		ImGui::Separator();
		ImGui::Checkbox("Debug", &m_bShow_Debug);
#endif
		ImGui::EndMenu();
	}
}

void App::onEvent(const SDL_Event& e) {
	if (!RenderState::IsExporting()) {
		switch (e.type) {

		case SDL_MOUSEMOTION:
			if (!ImGui::GetIO().WantCaptureMouse) {

			}
			break;

		case SDL_MOUSEWHEEL:
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (!ImGui::GetIO().WantCaptureMouse) {
				switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					break;
				case SDL_BUTTON_RIGHT:
					break;
				case SDL_BUTTON_MIDDLE:
					break;
				}
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (!ImGui::GetIO().WantCaptureMouse) {
				switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					break;
				case SDL_BUTTON_RIGHT:
					break;
				case SDL_BUTTON_MIDDLE:
					break;
				}
			}
			break;

		case SDL_KEYDOWN:
			if (!ImGui::GetIO().WantTextInput) {

			}
			break;

		case SDL_KEYUP:
			if (!ImGui::GetIO().WantTextInput) {

			}
			break;

		default:
			break;
		}
	}
}