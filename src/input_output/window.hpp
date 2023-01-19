#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <SDL.h>
#include <iostream>

namespace M3D
{
    namespace InputOutput
    {
        class Window
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
            Window();
            ~Window();

            // ----------------------------------------------------- GETTERS -------------------------------------------------------
            SDL_Window* get() const { return _window; }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void create(SDL_WindowFlags p_rendererTypeFlag);
            void captureEvents();
            // add loader d'audio 
            // add fonction qui permettent de supprimer des pistes
            // add fonction qui lis une piste / qui stop 

        private:
            // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
            SDL_Window* _window;
            // add audio => liste de piste preload au lancerment de la window + celles qui vont s'add ==> use set

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void _dispose();
        };
    }
}
#endif
