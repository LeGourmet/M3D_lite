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
        virtual ~KeyboardController() = default;

        // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
        virtual void receiveEvent(const SDL_Event &p_event) {
            switch (p_event.type) {
                case SDL_KEYDOWN:
                    _pressedButtons.emplace(p_event.key.keysym.scancode);
                    break;

                case SDL_KEYUP:
                    _pressedButtons.erase(p_event.key.keysym.scancode);
                    break;

                default:
                    break;
            }
        }

        virtual void clearEvents() { _pressedButtons.clear(); }

    protected:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        std::set<SDL_Scancode> _pressedButtons = std::set<SDL_Scancode>();

        // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
        bool _isKeyPressed(const SDL_Scancode &p_key) { return _pressedButtons.find(p_key) != _pressedButtons.end(); }
    };
}
}
#endif
