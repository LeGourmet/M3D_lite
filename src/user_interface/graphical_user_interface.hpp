#ifndef __GRAPHICAL_USER_INTERFACE_HPP__
#define __GRAPHICAL_USER_INTERFACE_HPP__

#include <SDL.h>

namespace M3D
{
namespace UserInterface
{
    class GraphicalUserInterface
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
        GraphicalUserInterface();
        ~GraphicalUserInterface();

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void init();
        bool captureEvent(SDL_Event p_event);
        void update(unsigned long long p_deltaTime);

    private:
        // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
        //ENUM state;
    };
}
}
#endif
