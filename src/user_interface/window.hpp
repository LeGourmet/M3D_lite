#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <SDL.h>

namespace M3D
{
    namespace UserInterface
    {
        class Window
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
            Window() {
                try{
                    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) // add flag
                        throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

                    SDL_WindowFlags engineTypeFlag;
                    switch (Application::getInstance().getRenderer().getType()) {
                        case ENGINE_TYPE::OPENGL : engineTypeFlag = SDL_WINDOW_OPENGL; break;
                        case ENGINE_TYPE::VULKAN : engineTypeFlag = SDL_WINDOW_VULKAN; break;
                        case ENGINE_TYPE::METAL : engineTypeFlag = SDL_WINDOW_METAL; break;
                        default: throw std::runtime_error("Non-supported renderer by this window!"); break;
                    }

                    _window = SDL_CreateWindow(
                        Application::getInstance().getTitle().c_str(),
                        SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED,
                        Application::getInstance().getWidth(),
                        Application::getInstance().getHeight(),
                        SDL_WINDOW_SHOWN | engineTypeFlag | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

                    if (_window == nullptr)
                        throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));
                }
                catch (const std::exception& p_e){
                    std::cerr << "Exception caught: " << std::endl << p_e.what() << std::endl;
                    _dispose();
                }
            }
            ~Window() { _dispose(); }

            // ----------------------------------------------------- GETTERS -------------------------------------------------------
            SDL_Window* get() const { return _window; }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void captureEvents() {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                    case SDL_WINDOWEVENT:
                        switch (event.window.event)
                        {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            Application::getInstance().getCamera().setScreenSize(event.window.data1, event.window.data2);
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
                        if(!Application::getInstance().getUserInterface().captureEvent(event))
                            Application::getInstance().getCamera().receiveEvent(event);
                        break;
                    }
                }
            }

        private:
            // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
            SDL_Window* _window = nullptr;

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void _dispose() {
                if (_window) SDL_DestroyWindow(_window);
                _window = nullptr;
                SDL_Quit();
            }
        };
    }
}
#endif
