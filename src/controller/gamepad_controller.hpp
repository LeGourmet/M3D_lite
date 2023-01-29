#ifndef __GAMEPAD_CONTROLLER_HPP__
#define __GAMEPAD_CONTROLLER_HPP__

#include <SDL_events.h>

namespace M3D
{
    namespace Controller
    {
        class GamepadController
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            GamepadController() {}
            ~GamepadController() {}

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void receiveEvent(const SDL_Event& p_event) {
                switch (p_event.type) {
                    default:
                        break;
                }
            }

            void clearEvents() {}

        protected:
        };
    }
}
#endif
