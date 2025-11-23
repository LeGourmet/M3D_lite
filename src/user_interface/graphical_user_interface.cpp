#include "graphical_user_interface.hpp"

#include "application.hpp"
#include "input_output/window.hpp"
#include "scene/objects/cameras/camera.hpp"
#include "scene/objects/lights/light.hpp"
#include "scene/scene_manager.hpp"
#include "renderer/renderer.hpp"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "portable-file-dialogs.h"

namespace M3D
{
	GraphicalUserInterface::GraphicalUserInterface(SDL_Window* p_window, const SDL_GLContext& p_glContext) {
		if (!IMGUI_CHECKVERSION()) throw std::runtime_error("Exception caught: IMGUI_CHECKVERSION() failed");
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();

		Vec3f BackgroundColor = Vec3f(50.f / 255.f);

		Vec3f TextColor = Vec3f(1.f);
		Vec3f TextDisableColor = Vec3f(150.f / 255.f);

		Vec3f ButtonColor = Vec3f(75.f / 255.f);
		Vec3f MenuBarColor = Vec3f(35.f / 255.f);
		Vec3f PopupColor = Vec3f(45.f / 255.f);
		Vec3f BorderColor = Vec3f(60.f / 255.f);
		Vec3f HeadColor = Vec3f(65.f / 255.f);

		Vec3f FrameColor = Vec3f(85.f / 255.f);
		Vec3f FrameHoverColor = Vec3f(120.f / 255.f);
		Vec3f FrameActiveColor = Vec3f(30.f / 255.f);
		Vec3f SliderColor = Vec3f(190.f / 255.f);

		Vec3f ProgressBarColor = Vec3f(63.f / 255.f, 128.f / 255.f, 200.f / 255.f);

		io.IniFilename = "resources/styles/default.ini";

		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_::ImGuiCol_Text] = ImVec4(TextColor.x, TextColor.y, TextColor.z, 1.f);
		style.Colors[ImGuiCol_::ImGuiCol_TextDisabled] = ImVec4(TextDisableColor.x, TextDisableColor.y, TextDisableColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_TextLink)] = ImVec4(TextColor.x, TextColor.y, TextColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_ScrollbarBg)] = ImVec4(0.f, 0.f, 0.f, 0.f);
		style.ScrollbarSize = 10.f;

		style.Colors[ImGuiCol_::ImGuiCol_MenuBarBg] = ImVec4(MenuBarColor.x, MenuBarColor.y, MenuBarColor.z, 1.f);
		style.Colors[ImGuiCol_::ImGuiCol_ChildBg] = ImVec4(BackgroundColor.x, BackgroundColor.y, BackgroundColor.z, 1.f);
		style.Colors[ImGuiCol_::ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
		style.WindowBorderSize = 0.f;

		style.Colors[ImGuiCol_::ImGuiCol_Border] = ImVec4(BorderColor.x, BorderColor.y, BorderColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_PopupBg)] = ImVec4(PopupColor.x, PopupColor.y, PopupColor.z, 1.f);
		style.PopupRounding = 3.f;

		style.FrameRounding = 4.f;
		style.FrameBorderSize = 0.f;
		style.Colors[(ImGuiCol_::ImGuiCol_FrameBg)] = ImVec4(FrameColor.x, FrameColor.y, FrameColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_FrameBgHovered)] = ImVec4(FrameHoverColor.x, FrameHoverColor.y, FrameHoverColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_FrameBgActive)] = ImVec4(FrameActiveColor.x, FrameActiveColor.y, FrameActiveColor.z, 1.f);

		style.GrabMinSize = 6.f;
		style.GrabRounding = 5.f;

		style.Colors[(ImGuiCol_::ImGuiCol_Header)] = ImVec4(MenuBarColor.x, MenuBarColor.y, MenuBarColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_HeaderHovered)] = ImVec4(HeadColor.x, HeadColor.y, HeadColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_HeaderActive)] = ImVec4(HeadColor.x, HeadColor.y, HeadColor.z, 1.f);

		style.ColorButtonPosition = ImGuiDir_Left;
		style.Colors[(ImGuiCol_::ImGuiCol_Button)] = ImVec4(FrameColor.x, FrameColor.y, FrameColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_ButtonHovered)] = ImVec4(FrameHoverColor.x, FrameHoverColor.y, FrameHoverColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_ButtonActive)] = ImVec4(FrameColor.x, FrameColor.y, FrameColor.z, 1.f);

		style.Colors[(ImGuiCol_::ImGuiCol_CheckMark)] = ImVec4(SliderColor.x, SliderColor.y, SliderColor.z, 1.f);

		style.Colors[(ImGuiCol_::ImGuiCol_SliderGrab)] = ImVec4(SliderColor.x, SliderColor.y, SliderColor.z, 1.f);
		style.Colors[(ImGuiCol_::ImGuiCol_SliderGrabActive)] = ImVec4(SliderColor.x, SliderColor.y, SliderColor.z, 1.f);

		style.Colors[(ImGuiCol_::ImGuiCol_PlotHistogram)] = ImVec4(ProgressBarColor.x, ProgressBarColor.y, ProgressBarColor.z, 1.f);

		style.SeparatorTextAlign = ImVec2(0.5f, 0.5f);
		style.SeparatorTextPadding = ImVec2(10.f, 6.f);

		style.CellPadding = ImVec2(10.f, 2.f);
		style.Colors[(ImGuiCol_::ImGuiCol_TableBorderLight)] = style.Colors[ImGuiCol_::ImGuiCol_Separator];

		io.Fonts->AddFontDefault();
		_font = io.Fonts->AddFontFromFileTTF("resources/styles/RobotoRegular.ttf", 16.f);

		if (!ImGui_ImplSDL3_InitForOpenGL(p_window, p_glContext)) throw std::runtime_error("ImGui_ImplSDL3_InitForOpenGL() failed");
		if (!ImGui_ImplOpenGL3_Init("#version 450")) throw std::runtime_error("ImGui_ImplOpenGL3_Init() failed");

		Application::getInstance().getSceneManager().loadNewScene("resources/assets/musee.glb");
	}

	GraphicalUserInterface::~GraphicalUserInterface() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		if (ImGui::GetCurrentContext() != nullptr)
			ImGui::DestroyContext();
	}

	void GraphicalUserInterface::pause() {}

	void GraphicalUserInterface::update(float p_deltaTime) {}

	void GraphicalUserInterface::drawFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushFont(_font);

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Load scene", ".glb")) {
					pfd::open_file openFileDialog = pfd::open_file("Choose file to read", pfd::path::home(), { "Text Files (.glb)", "*.glb", "All Files", "*" });
					for (std::filesystem::path path : openFileDialog.result())
						if (!path.empty())
							Application::getInstance().getSceneManager().loadNewScene(path);
				}

				if (ImGui::MenuItem("Add asset", ".glb")) {
					pfd::open_file openFileDialog = pfd::open_file("Choose files to read", pfd::path::home(), { "Text Files (.glb)", "*.glb", "All Files", "*" }, pfd::opt::multiselect);
					for (std::filesystem::path path : openFileDialog.result())
						if (!path.empty())
							Application::getInstance().getSceneManager().addAsset(path);
				}

				if (ImGui::MenuItem("Clean scene")) Application::getInstance().getSceneManager().clearScene();

				ImGui::Separator();

				if (ImGui::MenuItem("Save image", ".png")) {
					/*pfd::save_file saveFileDialog = pfd::save_file("Save screenshot", pfd::path::home(), {".png", "*.png", "All Files", "*"});
					std::filesystem::path path = saveFileDialog.result();
					if (!path.empty()) {
						std::vector<uchar> pixels(_viewportWidth * _viewportHeight * 3);
						glBindFramebuffer(GL_FRAMEBUFFER, _renderer.getViewportTextureID());
						glReadPixels(0, 0, _viewportWidth, _viewportHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
						glBindFramebuffer(GL_FRAMEBUFFER, 0);
						std::vector<uchar> flipped(pixels.size());
						for (uint y = 0; y < _viewportHeight; y++)
							std::memcpy(&flipped[y * _viewportWidth * 3], &pixels[(_viewportHeight - 1 - y) * _viewportWidth * 3], _viewportWidth * 3);
						lodepng::encode(path.string() + ".png", flipped, _viewportWidth, _viewportHeight, LCT_RGB);
					}*/
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) Application::getInstance().stop();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit", false)) {
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window")) {
				if (ImGui::MenuItem("Fullscreen", Application::getInstance().getWindow().isFullscreen() ? "ON" : "OFF")) Application::getInstance().getWindow().switchFullScreenToMaximized();

				ImGui::Separator();

				ImGui::MenuItem("Width", std::to_string(Application::getInstance().getWidth()).c_str(), false, false);
				ImGui::MenuItem("Height", std::to_string(Application::getInstance().getHeight()).c_str(), false, false);
				if (ImGui::MenuItem("VSync", (Application::getInstance().getWindow().isVSync() ? "ON" : "OFF"))) Application::getInstance().getWindow().toggleVSync();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				ImGui::TextLinkOpenURL("Manual", "https://github.com/LeGourmet/M3D_lite");
				ImGui::Separator();
				ImGui::TextLinkOpenURL("Report a bug", "https://github.com/LeGourmet/M3D_lite/issues");
				ImGui::EndMenu();
			}

			ImGui::Text("%.0f FPS", io.Framerate);

			ImGui::EndMainMenuBar();
		}

		float barHeight = ImGui::GetFrameHeight();
		float canvaWidth = (_hide ? (float)(Application::getInstance().getWidth()) : (float)(Application::getInstance().getWidth())*4.f/5.f);
		float canvaHeight = glm::max(0.f, (float)(Application::getInstance().getHeight()) - barHeight);

		ImGui::SetNextWindowSize(ImVec2(glm::max(0.f, glm::ceil((float)(Application::getInstance().getWidth())-canvaWidth + barHeight)), canvaHeight));
		ImGui::SetNextWindowPos(ImVec2(glm::max(0.f, canvaWidth - barHeight), barHeight));
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (ImGui::Begin("##Settings", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImVec2 buttonSize = ImVec2(barHeight, 3.f*barHeight);
			ImVec2 buttonPos = ImVec2(3, glm::max(0.f, 0.5f*(Application::getInstance().getHeight()-barHeight-buttonSize.y)));

			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, buttonPos);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].x, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].y, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].z, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].x, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].y, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].z, 1.f));
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].x, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].y, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ChildBg].z, 0.85f));
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_MenuBarBg].x, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_MenuBarBg].y, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_MenuBarBg].z, 1.f));
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::BeginChild("##Settings_icones", ImVec2(barHeight, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse);

			if (ImGui::Button("##Settings_button", buttonSize)) { _hide = !_hide; }
			ImVec2 p = ImGui::GetItemRectMin();
			ImVec2 center = ImVec2(p.x + buttonSize.x * 0.5f, p.y + buttonSize.y * 0.5f);
			if (_hide) { ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2(center.x + 5, center.y + 5), ImVec2(center.x - 5, center.y), ImVec2(center.x + 5, center.y - 5), ImGui::GetColorU32(ImGuiCol_Text)); }
			else { ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2(center.x - 5, center.y - 5), ImVec2(center.x + 5, center.y), ImVec2(center.x - 5, center.y + 5), ImGui::GetColorU32(ImGuiCol_Text)); }

			ImGui::EndChild();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(5);
			
			ImGui::SameLine();

			ImGui::BeginChild("##Settings_options", ImGui::GetContentRegionAvail());

			ImGui::SeparatorText("Movements");
			float movSensi = Application::getInstance().getSceneManager().getMovementSensitivity() * 100.f;
			if (ImGui::SliderFloat("movement sensitivity", &movSensi, 0.f, 1000.f, "%0.1f")) Application::getInstance().getSceneManager().setMovementSensitivity(movSensi * 0.01f);

			float rotSensi = Application::getInstance().getSceneManager().getRotationSensitivity() * 1000.f;
			if (ImGui::SliderFloat("rotation sensitivity", &rotSensi, 0.f, 10.f, "%0.1f")) Application::getInstance().getSceneManager().setRotationSensitivity(rotSensi * 0.001f);

			for (uint i = 0; i < Application::getInstance().getSceneManager().getCameras().size();i++) {
				std::string cType = Application::getInstance().getSceneManager().getCameras()[i].getType() == CAMERA_TYPE::PERSPECTIVE ? "Perspective" :
					Application::getInstance().getSceneManager().getCameras()[i].getType() == CAMERA_TYPE::ORTHOGRAPHIC ? "Orthographic" : "Other";

				ImGui::SeparatorText((cType + std::string(" camera")).c_str());

				if (ImGui::BeginTable("##Camera_options_" + i, 2, ImGuiTableFlags_::ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_::ImGuiTableFlags_SizingStretchSame)) {
					if (cType == "Perspective") {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Fovy");

						ImGui::TableSetColumnIndex(1);

					}
					else if (cType == "Orthographic") {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Scale");

						ImGui::TableSetColumnIndex(1);

					}
					ImGui::EndTable();
				}
			}

			for (uint i = 0; i < Application::getInstance().getSceneManager().getLights().size();i++) {
				std::string lType = Application::getInstance().getSceneManager().getLights()[i].getType() == LIGHT_TYPE::DIRECTIONAL ? "Sun" :
					Application::getInstance().getSceneManager().getLights()[i].getType() == LIGHT_TYPE::POINT ? "Point" :
					Application::getInstance().getSceneManager().getLights()[i].getType() == LIGHT_TYPE::SPOT ? "Spot" : "Other";

				ImGui::SeparatorText((lType + std::string(" light")).c_str());

				if (ImGui::BeginTable("##Lights_options_" + i, 2, ImGuiTableFlags_::ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_::ImGuiTableFlags_SizingStretchSame)) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Color");

					ImGui::TableSetColumnIndex(1);
					float lightColor[3] = { Application::getInstance().getSceneManager().getLights()[i].getColor().x, Application::getInstance().getSceneManager().getLights()[i].getColor().y, Application::getInstance().getSceneManager().getLights()[i].getColor().z };
					//if (ImGui::ColorEdit3("##Light_color_" + i, lightColor)) _scene.getLights()[i]->setColor(Vec3f(lightColor[0], lightColor[1], lightColor[2]));

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Intensity");

					ImGui::TableSetColumnIndex(1);
					float lightIntensity = Application::getInstance().getSceneManager().getLights()[i].getIntensity();
					//if (ImGui::DragFloat("##Light_intensity_1", &lightIntensity)) _scene.getLights()[i]->setIntensity(lightIntensity);

					if (lType == "Sun") {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						ImGui::TableSetColumnIndex(1);

					}
					else if (lType == "Point") {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						ImGui::TableSetColumnIndex(1);

					}
					else if (lType == "Spot") {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Falloff");

						ImGui::TableSetColumnIndex(1);

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Falloff Angle");

						ImGui::TableSetColumnIndex(1);

					}

					ImGui::EndTable();
				}
			}

			ImGui::SeparatorText("Renderer");

			ImGui::Text("Anti-Aliasing");
			ImGui::SameLine();
			if (ImGui::RadioButton("NONE", Application::getInstance().getRenderer().getAAType() == AA_TYPE::NONE)) Application::getInstance().getRenderer().setAAType(AA_TYPE::NONE);
			ImGui::SameLine();
			if (ImGui::RadioButton("FXAA", Application::getInstance().getRenderer().getAAType() == AA_TYPE::FXAA)) Application::getInstance().getRenderer().setAAType(AA_TYPE::FXAA);
			ImGui::SameLine();
			if (ImGui::RadioButton("SMAA", Application::getInstance().getRenderer().getAAType() == AA_TYPE::SMAA)) Application::getInstance().getRenderer().setAAType(AA_TYPE::SMAA);

			//+ float bloom ?

			ImGui::EndChild();
		}
		ImGui::End();
		ImGui::PopStyleVar(2);

		ImGui::PopFont();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	bool GraphicalUserInterface::captureEvent(const SDL_Event& event) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplSDL3_ProcessEvent(&event);
		return (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput);
	}

	void GraphicalUserInterface::clearEvents() {
		//(set screen pause / do nothing if scene isn't display)
	}

	void GraphicalUserInterface::resize(const uint p_width, const uint p_height) {
		// nothing
	}
}