#ifndef __CONTROLLER_KEYBOARD_CONTROLLER__
#define __CONTROLLER_KEYBOARD_CONTROLLER__

#include <SDL_events.h>
#include <set>

namespace TutoVulkan
{
namespace Controller
{
    class KeyboardController
    {
    public:
        KeyboardController() {}

        virtual ~KeyboardController() = default;

        virtual void receiveEvent(const SDL_Event &p_event)
        {
            switch (p_event.type)
            {
            case SDL_KEYDOWN:
            {
                _handleKeyDownEvent(p_event.key.keysym.scancode);
                if (p_event.key.repeat == 0)
                    _handleKeyPressedEvent(p_event.key.keysym.scancode);
                break;
            }

            case SDL_KEYUP:
                _handleKeyUpEvent(p_event.key.keysym.scancode);
                break;

            default:
                break;
            }
        }

        virtual void clearEvents() { _pressedButtons.clear(); }

    protected:
        std::set<SDL_Scancode> _pressedButtons = std::set<SDL_Scancode>();

        void _handleKeyDownEvent(const SDL_Scancode &p_key) { _pressedButtons.emplace(p_key); };
        void _handleKeyUpEvent(const SDL_Scancode &p_key) { _pressedButtons.erase(p_key); };
        static void _handleKeyPressedEvent(const SDL_Scancode &p_key) {}

        bool _isKeyPressed(const SDL_Scancode &p_key) { return _pressedButtons.find(p_key) != _pressedButtons.end(); }
    };
} // namespace Controller
} // namespace TutoVulkan
#endif
