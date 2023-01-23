#include "window.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "user_interface/graphical_user_interface.hpp"

namespace M3D
{
    namespace InputOutput
    {
        Window::Window() {}
        Window::~Window() { _dispose(); }

        void Window::create(SDL_WindowFlags p_rendererTypeFlag) {
            try {
                if (SDL_Init(SDL_INIT_VIDEO) != 0) // add flag SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO
                    throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

                _window = SDL_CreateWindow(
                    Application::getInstance().getTitle().c_str(),
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    Application::getInstance().getWidth(),
                    Application::getInstance().getHeight(),
                    SDL_WINDOW_SHOWN | p_rendererTypeFlag | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

                if (_window == nullptr)
                    throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));
            }
            catch (const std::exception& p_e) {
                std::cerr << "Exception caught: " << std::endl << p_e.what() << std::endl;
                _dispose();
            }
        }

        void Window::captureEvents() {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_WINDOWEVENT: // ADD EVENT RESPONSES 
                    switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        Application::getInstance().resize(event.window.data1, event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        Application::getInstance().stop();
                        break;
                    default:
                        break;
                    }
                    break;

                case SDL_QUIT:
                    Application::getInstance().stop();
                    break;

                default:
                    if (!Application::getInstance().getGraphicalUserInterface().captureEvent(event))
                        Application::getInstance().getSceneManager().captureEvent(event);
                    break;
                }
            }
        }

        void Window::_dispose() {
            if (_window) SDL_DestroyWindow(_window);
            _window = nullptr;
            SDL_Quit();
        }
    }
}