#include "user_interface.hpp"

#include <SDL.h>
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui_impl_vulkan.h"
#include <glm/gtx/string_cast.hpp>

#include "application.hpp"
#include "controller/freefly_camera.hpp"
#include "engine/vulkan_engine.hpp"
#include "model/scene.hpp"
#include "utils/constant.hpp"

#include <iostream>


namespace M3D
{
namespace UI
{
    UserInterface::UserInterface()
    {
        try
        {
            std::cout << "Init SDL2 ..." << std::endl;
            _initSDL2();
            std::cout << "Done !" << std::endl;
            std::cout << "Init IMGUI ..." << std::endl;
            _initIMGUI();
            std::cout << "Done !" << std::endl;
        }
        catch (const std::exception &p_e)
        {
            std::cerr << "Exception caught: " << std::endl
                      << p_e.what() << std::endl;
            _disposeAll();
        }
    }

    UserInterface::~UserInterface() { _disposeAll(); }

    void UserInterface::_initSDL2()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
            throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

        _window = SDL_CreateWindow(
            Application::getInstance().getTitle().c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            Application::getInstance().getWidth(),
            Application::getInstance().getHeight(),
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        if (_window == nullptr)
            throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));
    }

    void UserInterface::_initIMGUI()
    {
        if (!IMGUI_CHECKVERSION())
            throw std::runtime_error("Exception caught: IMGUI_CHECKVERSION() failed");

        ImGui::CreateContext();

        // Setup controls.
        ImGuiIO &io = ImGui::GetIO();
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

    void UserInterface::_disposeAll()
    {
        _disposeIMGUI();
        _disposeSDL2();
    }

    void UserInterface::_disposeSDL2()
    {
        if (_window)
            SDL_DestroyWindow(_window);
        _window = nullptr;
        SDL_Quit();
    }

    void UserInterface::_disposeIMGUI()
    {
        Application::getInstance().getRenderer().waitIdle();
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        if (ImGui::GetCurrentContext() != nullptr)
            ImGui::DestroyContext();
        Application::getInstance().getRenderer().cleanup();
    }

    void UserInterface::update()
    {
        ImGuiIO &io = ImGui::GetIO();

        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (!(io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput))
                App::getInstance().getCamera().receiveEvent(event);
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    App::getInstance().getCamera().setScreenSize(event.window.data1, event.window.data2);
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    App::getInstance().stop();
                    break;
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                App::getInstance().stop();
                break;
            default:
                break;
            }
        }

        // New frame.
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame(_window);
        ImGui::NewFrame();

        // Configuration.
        ImGuiViewport *viewport = ImGui::GetMainViewport();
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
                _openFileDialog = std::shared_ptr<pfd::open_file>(new pfd::open_file("Choose File", "", {"All Files", "*"}, pfd::opt::multiselect));

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
    }

} // namespace UI
} // namespace TutoVulkan
