#ifndef __MOUSE_CONTROLLER_HPP__
#define __MOUSE_CONTROLLER_HPP__

#include <SDL_events.h>

namespace M3D
{
namespace Controller
{
    class MouseController
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        MouseController() {}
        virtual ~MouseController() = default;

        // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
        virtual void receiveEvent(const SDL_Event &p_event){
            switch (p_event.type){
                case SDL_MOUSEBUTTONDOWN:
                    switch (p_event.button.button){
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
                    break;

                case SDL_MOUSEBUTTONUP:
                    switch (p_event.button.button){
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
                    break;

                case SDL_MOUSEMOTION:
                    _deltaMousePositionX = p_event.motion.xrel;
                    _deltaMousePositionY = p_event.motion.yrel;
                    break;

                case SDL_MOUSEWHEEL:
                    _deltaMouseWheel = p_event.wheel.y;
                    break;

                default:
                    break;
            }
        }

        virtual void clearEvents(){
            _mouseLeftPressed = false;
            _mouseRightPressed = false;
            _mouseMiddlePressed = false;
            _deltaMousePositionX = 0;
            _deltaMousePositionY = 0;
            _deltaMouseWheel = 0;
        }

    protected:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        bool _mouseLeftPressed = false;
        bool _mouseRightPressed = false;
        bool _mouseMiddlePressed = false;
        int  _deltaMousePositionX = 0;
        int  _deltaMousePositionY = 0;
        int _deltaMouseWheel = 0;
    };
}
}
#endif
