#ifndef __INPUT_MANAGER_HPP__
#define __INPUT_MANAGER_HPP__

#include <SDL.h>

namespace M3D
{
    namespace UserInterface
    {
        class InputManager
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
            InputManager() { }
            ~InputManager() { }

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
                        if (!Application::getInstance().getGraphicalUserInterface().captureEvent(event))
                            Application::getInstance().getScene().receiveEvent(event);
                        break;
                    }
                }
            }
        };
    }
}
#endif

