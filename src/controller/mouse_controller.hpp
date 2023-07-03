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
            MouseController() { clearEvents(); }
            ~MouseController() {}

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void receiveEvent(const SDL_Event &p_event){
                switch (p_event.type){
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
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
                            default: break;
                        }
                        break;

                    case SDL_EVENT_MOUSE_BUTTON_UP:
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
                            default: break;
                        }
                        break;

                    case SDL_EVENT_MOUSE_MOTION:
                        _deltaMousePositionX = p_event.motion.xrel;
                        _deltaMousePositionY = p_event.motion.yrel;
                        break;

                    case SDL_EVENT_MOUSE_WHEEL:
                        _deltaMouseWheel = p_event.wheel.y;
                        break;

                    default: break;
                }
            }

            void clearEvents(){
                _mouseLeftPressed = false;
                _mouseRightPressed = false;
                _mouseMiddlePressed = false;
                _deltaMousePositionX = 0.f;
                _deltaMousePositionY = 0.f;
                _deltaMouseWheel = 0.f;
            }

        protected:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            bool _mouseLeftPressed;
            bool _mouseRightPressed;
            bool _mouseMiddlePressed;
            float _deltaMousePositionX;
            float _deltaMousePositionY;
            float _deltaMouseWheel;
        };
    }
}

#endif
