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
            
            void chronoUpdate();
            unsigned long long getDeltaTime();
            // add loader d'audio 
            // add fonction qui permettent de supprimer des pistes
            // add fonction qui lis une piste / qui stop 

            /*
            SDLGetWindowRefreshRate(SDL_Window *Window)
            {
                SDL_DisplayMode Mode;
                int DisplayIndex = SDL_GetWindowDisplayIndex(Window);
                // If we can't find the refresh rate, we'll return this:
                int DefaultRefreshRate = 60;
                if (SDL_GetDesktopDisplayMode(DisplayIndex, &Mode) != 0)
                {
                    return DefaultRefreshRate;
                }
                if (Mode.refresh_rate == 0)
                {
                    return DefaultRefreshRate;
                }
                return Mode.refresh_rate;
            }
            */

        private:
            // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
            SDL_Window* _window = nullptr;
            unsigned long long _time = 0;   // SDL ticks (ms) bias
            // screen refresh rate (warning plusieur écrans)
            // add audio => liste de piste preload au lancerment de la window + celles qui vont s'add ==> use set

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            bool _captureEvent(SDL_Event p_event);
            void _switchFullScreen2Maximized();
            void _takeScreenShot();
            void _dispose();
        };
    }
}
#endif
