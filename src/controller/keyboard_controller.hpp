#ifndef __KEYBOARD_CONTROLLER_HPP__
#define __KEYBOARD_CONTROLLER_HPP__

#include <SDL_events.h>

#include <set>

namespace M3D
{
    namespace Controller
    {
        class KeyboardController
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            KeyboardController() {}
            ~KeyboardController() {}

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void receiveEvent(const SDL_Event &p_event) {
                switch (p_event.type) {
                    case SDL_EVENT_KEY_DOWN:
                        _pressedButtons.insert(p_event.key.keysym.scancode);
                        break;

                    case SDL_EVENT_KEY_UP:
                        _pressedButtons.erase(p_event.key.keysym.scancode);
                        break;

                    default: break;
                }
            }

            void clearEvents() { _pressedButtons.clear(); }

        protected:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            std::set<SDL_Scancode> _pressedButtons = std::set<SDL_Scancode>();

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            bool _isKeyPressed(const SDL_Scancode &p_key) { return _pressedButtons.find(p_key) != _pressedButtons.end(); }
        };
    }
}

#endif
