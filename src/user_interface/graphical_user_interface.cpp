#include "graphical_user_interface.hpp"

#include "application.hpp"
#include "input_output/window.hpp"
#include "scene/objects/cameras/camera.hpp"
#include "scene/objects/lights/light.hpp"
#include "scene/scene_graph/scene_graph_node.hpp"
#include "scene/scene_manager.hpp"
#include "renderer/renderer.hpp"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "portable-file-dialogs.h"

#include <iostream>

namespace M3D
{
namespace UserInterface
{
	GraphicalUserInterface::GraphicalUserInterface() { Application::getInstance().getSceneManager().loadNewScene("assets/musee.gltf"); }
	GraphicalUserInterface::GraphicalUserInterface(SDL_Window* p_window, const SDL_GLContext& p_glContext) {
		try
		{
			// create context
			if (!IMGUI_CHECKVERSION()) throw std::runtime_error("Exception caught: IMGUI_CHECKVERSION() failed");

			ImGui::CreateContext();

			// setup controls
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			// style
			ImGui::StyleColorsDark();
			ImGui::GetStyle().WindowRounding = 0.f;
			ImGui::GetStyle().ChildRounding = 0.f;
			ImGui::GetStyle().FrameRounding = 0.f;
			ImGui::GetStyle().GrabRounding = 0.f;
			ImGui::GetStyle().PopupRounding = 0.f;
			ImGui::GetStyle().ScrollbarRounding = 0.f;
			ImGui::GetStyle().WindowBorderSize = 0.f;
			ImGui::GetStyle().WindowPadding = ImVec2(0.f, 0.f);

			// init backends
			if (!ImGui_ImplSDL3_InitForOpenGL(p_window, p_glContext)) throw std::runtime_error("ImGui_ImplSDL3_InitForOpenGL() failed");
			if (!ImGui_ImplOpenGL3_Init("#version 450")) throw std::runtime_error("ImGui_ImplOpenGL3_Init() failed");
		}
		catch (const std::exception& p_e) {
			std::cerr << "Exception caught: " << std::endl << p_e.what() << std::endl;
			_dispose();
		}

		//Application::getInstance().getSceneManager().loadNewScene("assets/musee.glb");
		Application::getInstance().getSceneManager().loadNewScene("assets/sponza.glb");
		//Application::getInstance().getSceneManager().loadNewScene("assets/emissive.glb");
	}

	GraphicalUserInterface::~GraphicalUserInterface() { _dispose(); }

	void GraphicalUserInterface::pause() { }
	
	void GraphicalUserInterface::update(unsigned long long p_deltaTime) { }

	void GraphicalUserInterface::drawFrame() { 
		if (_hide) return;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("ScreenSettings")) {
				int targetFPS = Application::getInstance().getWindow().getTargetFps();
				if (ImGui::InputInt("Target FPS", &targetFPS, 1, 10)) Application::getInstance().getWindow().setTargetFPS(targetFPS);

				bool vSync = Application::getInstance().getWindow().getVSync();
				if (ImGui::Checkbox("V-Sync", &vSync)) Application::getInstance().getWindow().setVSync(vSync);

				ImGui::EndMenu();
			}

			ImGui::Text("FPS: %.0f", io.Framerate);

			ImGui::EndMainMenuBar();
		}

		if (ImGui::Begin("Settings")) {
			if (ImGui::BeginTabBar("#settings_tab_bar")) {
				if (ImGui::BeginTabItem("Scene")) {
					if (ImGui::Button("clear scene")) Application::getInstance().getSceneManager().clearScene();
					ImGui::SameLine();
					if (ImGui::Button("load scene")) {
						pfd::open_file openFileDialog = pfd::open_file("Choose file to read", pfd::path::home(), { "Text Files (.glb)", "*.glb", "All Files", "*" });
						for (std::filesystem::path path : openFileDialog.result())
							if (!path.empty())
								Application::getInstance().getSceneManager().loadNewScene(path);
					}
					ImGui::SameLine();
					if (ImGui::Button("add asset")) {
						pfd::open_file openFileDialog = pfd::open_file("Choose files to read", pfd::path::home(), { "Text Files (.glb)", "*.glb", "All Files", "*" }, pfd::opt::multiselect);
						for (std::filesystem::path path : openFileDialog.result())
							if (!path.empty())
								Application::getInstance().getSceneManager().addAsset(path);
					}

					ImGui::Separator();

					ImGui::TextColored(ImVec4(1, 1, 0, 1), "Camera");

					if (ImGui::RadioButton("PERSPECTIVE", Application::getInstance().getSceneManager().getMainCamera().getType() == CAMERA_TYPE::PERSPECTIVE))
						Application::getInstance().getSceneManager().getMainCamera().setCameraType(CAMERA_TYPE::PERSPECTIVE);
					ImGui::SameLine();
					if (ImGui::RadioButton("ORTHOGRAPHIC", Application::getInstance().getSceneManager().getMainCamera().getType() == CAMERA_TYPE::ORTHOGRAPHIC))
						Application::getInstance().getSceneManager().getMainCamera().setCameraType(CAMERA_TYPE::ORTHOGRAPHIC);

					float znear = Application::getInstance().getSceneManager().getMainCamera().getZNear();
					if (ImGui::SliderFloat("znear", &znear, 0.f, 1000.f, "%0.1f")) Application::getInstance().getSceneManager().getMainCamera().setZNear(znear);

					float zfar = Application::getInstance().getSceneManager().getMainCamera().getZFar();
					if (ImGui::SliderFloat("zfar", &zfar, 0.f, 1000.f, "%0.1f")) Application::getInstance().getSceneManager().getMainCamera().setZFar(zfar);

					switch (Application::getInstance().getSceneManager().getMainCamera().getType()) {
					case CAMERA_TYPE::PERSPECTIVE: {
						float fovy = Application::getInstance().getSceneManager().getMainCamera().getFovy();
						if (ImGui::SliderFloat("fovy", &fovy, 0.f, PIf, "%0.01f")) Application::getInstance().getSceneManager().getMainCamera().setFovy(fovy);
						break;
					}
					case CAMERA_TYPE::ORTHOGRAPHIC: {
						float xmag = Application::getInstance().getSceneManager().getMainCamera().getXMag();
						if (ImGui::SliderFloat("xmag", &xmag, 1.f, 2000.f, "%1.f")) Application::getInstance().getSceneManager().getMainCamera().setXMag(xmag);

						float ymag = Application::getInstance().getSceneManager().getMainCamera().getYMag();
						if (ImGui::SliderFloat("ymag", &ymag, 1.f, 2000.f, "%1.f")) Application::getInstance().getSceneManager().getMainCamera().setYMag(ymag);
						break;
					}
					}

					ImGui::Separator();

					ImGui::TextColored(ImVec4(1, 1, 0, 1), "Lights");

					/*if (ImGui::Button("Add sun light")) {
						Scene::Light* sun = new Scene::Light(LIGHT_TYPE::DIRECTIONAL, VEC3F_ONE, 1000.f);
						Application::getInstance().getSceneManager().addLight(sun);
						Application::getInstance().getSceneManager().addInstance(sun, new Scene::SceneGraphNode(nullptr, VEC3F_ZERO, VEC3F_ONE, QUATF_ID));
					}*/

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Renderer")) {

					ImGui::TextColored(ImVec4(1, 1, 0, 1), "Anti-Aliasing");

					if (ImGui::RadioButton("NONE", Application::getInstance().getRenderer().getAAType() == AA_TYPE::NONE)) { Application::getInstance().getRenderer().setAAType(AA_TYPE::NONE); }
					ImGui::SameLine();
					if (ImGui::RadioButton("FXAA", Application::getInstance().getRenderer().getAAType() == AA_TYPE::FXAA)) { Application::getInstance().getRenderer().setAAType(AA_TYPE::FXAA); }
					ImGui::SameLine();
					if (ImGui::RadioButton("SMAA", Application::getInstance().getRenderer().getAAType() == AA_TYPE::SMAA)) { Application::getInstance().getRenderer().setAAType(AA_TYPE::SMAA); }

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			ImGui::End();
		}

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

	void GraphicalUserInterface::resize(const int p_width, const int p_height) {
		// nothing
	}

	void GraphicalUserInterface::_dispose() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		if (ImGui::GetCurrentContext() != nullptr)
			ImGui::DestroyContext();
	}

	void GraphicalUserInterface::toggleDisplayMode() { _hide = !_hide; }
}
}