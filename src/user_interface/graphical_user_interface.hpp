#ifndef __GRAPHICAL_USER_INTERFACE_HPP__
#define __GRAPHICAL_USER_INTERFACE_HPP__

#include <SDL.h>

namespace M3D
{
namespace UserInterface
{
    class GraphicalUserInterface
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
        GraphicalUserInterface() {
            /*try {
                if (!IMGUI_CHECKVERSION())
                    throw std::runtime_error("Exception caught: IMGUI_CHECKVERSION() failed");

                ImGui::CreateContext();

                // Setup controls.
                ImGuiIO& io = ImGui::GetIO();
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

                // Style.
                ImGui::StyleColorsDark();
                ImGui::GetStyle().WindowRounding = 0.f;
                ImGui::GetStyle().ChildRounding = 0.f;
                ImGui::GetStyle().FrameRounding = 0.f;
                ImGui::GetStyle().GrabRounding = 0.f;
                ImGui::GetStyle().PopupRounding = 0.f;
                ImGui::GetStyle().ScrollbarRounding = 0.f;
                ImGui::GetStyle().WindowBorderSize = 0.f;
                ImGui::GetStyle().WindowPadding = ImVec2(0.f, 0.f);

                // .ini location.
                io.IniFilename = IMGUI_INI_FILE.c_str();
            }
            catch (const std::exception& p_e)
            {
                std::cerr << "Exception caught: " << std::endl << p_e.what() << std::endl;
                _dispose();
            }*/
        }

        ~GraphicalUserInterface() { _dispose(); }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        bool captureEvent(SDL_Event event) {
            /*ImGuiIO& io = ImGui::GetIO();
            ImGui_ImplSDL2_ProcessEvent(&event);

            // New frame.
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL2_NewFrame(_window);
            ImGui::NewFrame();

            // Configuration.
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::SetNextWindowBgAlpha(0.0f);

            // Draw menu.
            if (ImGui::BeginMainMenuBar())
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));

                // Open obj
                if (ImGui::MenuItem("Open"))
                    _openFileDialog = std::shared_ptr<pfd::open_file>(new pfd::open_file("Choose File", "", { "All Files", "*" }, pfd::opt::multiselect));

                // FPS.
                ImGui::Text("FPS: %.0f", io.Framerate);

                ImGui::PopStyleVar();
                ImGui::EndMainMenuBar();
            }

            ImGui::Begin("Settings");

            // Background.
            if (ImGui::ColorEdit3("clear color", glm::value_ptr(CLEAR_COLOR)))
                App::getInstance().getRenderer().setClearColor(CLEAR_COLOR);

            ImGui::Separator();

            // Camera.
            ImGui::SliderFloat("fovy", App::getInstance().getCamera().getFov(), 10.f, 180.f, "%1.f");
            App::getInstance().getCamera().setFov(*App::getInstance().getCamera().getFov());

            ImGui::SliderFloat("zNear", App::getInstance().getCamera().getNear(), 0.f, 1.f, "%.1f");
            App::getInstance().getCamera().setNear(*App::getInstance().getCamera().getNear());

            ImGui::SliderFloat("zFar", App::getInstance().getCamera().getFar(), 10.f, 10000.f, "%1.f");
            App::getInstance().getCamera().setFar(*App::getInstance().getCamera().getFar());

            ImGui::SliderFloat("speed", App::getInstance().getCamera().getSpeed(), 1.f, 200.f, "%1.f");

            if (ImGui::Button("Recenter"))
                App::getInstance().getCamera().reset();

            ImGui::End();

            // Open file dialog.
            if (_openFileDialog && _openFileDialog->ready())
            {
                std::vector<std::string> files = _openFileDialog->result();

                if (!files.empty())
                    App::getInstance().getScene().loadFileTriangleMesh(files[0]);

                _openFileDialog = nullptr;
                App::getInstance().getCamera().clearEvents();
            }

            ImGui::Render();

            return (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput);*/
            return false;
        }

    private:
        // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
        //std::shared_ptr<pfd::open_file> _openFileDialog = nullptr;
        //TRUC_ENUM state;

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _dispose() {
            /*Application::getInstance().getRenderer().waitIdle();
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            if (ImGui::GetCurrentContext() != nullptr)
                ImGui::DestroyContext();
            Application::getInstance().getRenderer().cleanup();*/
        }
    };
}
}
#endif
