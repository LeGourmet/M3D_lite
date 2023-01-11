#ifndef __CONTROLLER_MOUSE_CONTROLLER__
#define __CONTROLLER_MOUSE_CONTROLLER__

#include <SDL_events.h>

namespace TutoVulkan
{
namespace Controller
{
    class MouseController
    {
    public:
        MouseController() {}
        virtual ~MouseController() = default;

        virtual void receiveEvent(const SDL_Event &p_event)
        {
            switch (p_event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                _handleMouseButtonDownEvent(p_event.button);
                break;
            case SDL_MOUSEBUTTONUP:
                _handleMouseButtonUpEvent(p_event.button);
                break;
            case SDL_MOUSEMOTION:
                _handleMouseMotionEvent(p_event.motion);
                break;
            case SDL_MOUSEWHEEL:
                _handleMouseWheelEvent(p_event.wheel);
                break;
            default:
                break;
            }
        }

        virtual void clearEvents()
        {
            _mouseLeftPressed = false;
            _mouseRightPressed = false;
            _mouseMiddlePressed = false;
            _deltaMousePosition.x = 0;
            _deltaMousePosition.y = 0;
            _deltaMouseWheel = 0;
        }

    protected:
        bool _mouseLeftPressed = false;
        bool _mouseRightPressed = false;
        bool _mouseMiddlePressed = false;
        Vec2i _deltaMousePosition = Vec2i();
        int _deltaMouseWheel = 0;

        void _handleMouseButtonDownEvent(const SDL_MouseButtonEvent &p_event)
        {
            switch (p_event.button)
            {
            case SDL_BUTTON_LEFT:
                _mouseLeftPressed = true;
                break;
            case SDL_BUTTON_RIGHT:
                _mouseRightPressed = true;
                break;
            case SDL_BUTTON_MIDDLE:
                _mouseMiddlePressed = true;
                break;
            default:
                break;
            }
        }

        void _handleMouseButtonUpEvent(const SDL_MouseButtonEvent &p_event)
        {
            switch (p_event.button)
            {
            case SDL_BUTTON_LEFT:
                _mouseLeftPressed = false;
                break;
            case SDL_BUTTON_RIGHT:
                _mouseRightPressed = false;
                break;
            case SDL_BUTTON_MIDDLE:
                _mouseMiddlePressed = false;
                break;
            default:
                break;
            }
        }

        void _handleMouseMotionEvent(const SDL_MouseMotionEvent &p_event)
        {
            _deltaMousePosition.x = p_event.xrel;
            _deltaMousePosition.y = p_event.yrel;
        }

        void _handleMouseWheelEvent(const SDL_MouseWheelEvent &p_event)
        {
            _deltaMouseWheel = p_event.y;
        }
    };
} // namespace Controller
} // namespace TutoVulkan
#endif
