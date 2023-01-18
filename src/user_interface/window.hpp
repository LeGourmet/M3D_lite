#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <SDL.h>
#include <iostream>

namespace M3D
{
    namespace UserInterface
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

        private:
            // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
            SDL_Window* _window;

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void _dispose();
        };
    }
}
#endif
