#ifndef __GRAPHICAL_USER_INTERFACE_HPP__
#define __GRAPHICAL_USER_INTERFACE_HPP__

#include <SDL_events.h>

namespace M3D
{
    namespace UserInterface
    {
        class GraphicalUserInterface
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            GraphicalUserInterface(SDL_Window* p_window, const SDL_GLContext& p_glContext);
            ~GraphicalUserInterface();

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void pause();
            void resize(const int p_width, const int p_height);
            void update(float p_deltaTime);
            void drawFrame();

            bool captureEvent(const SDL_Event& p_event);
            void clearEvents();

            void toggleDisplayMode();

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            bool _hide = false;

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void _dispose();
        };
    }
}

#endif
