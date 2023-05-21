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
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
        GraphicalUserInterface();
        GraphicalUserInterface(SDL_Window* p_window, const SDL_GLContext& p_glContext);
        ~GraphicalUserInterface();

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void pause();
        void update(unsigned long long p_deltaTime);
        void drawFrame();
        
        bool captureEvent(const SDL_Event& p_event);
        void clearEvents();

    private:
        // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
        void _dispose();
    };
}
}
#endif
