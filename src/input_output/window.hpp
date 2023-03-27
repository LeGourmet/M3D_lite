#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <SDL_events.h>
#include <SDL_video.h>

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
            inline SDL_Window& get() const { return *_window; }
            inline bool getVSync() { return _vSync; }

            // ----------------------------------------------------- SETTERS -------------------------------------------------------
            void setVSync(bool p_vSync) { 
                _vSync = p_vSync; 
                if (SDL_WINDOW_OPENGL) { SDL_GL_SetSwapInterval(p_vSync); }
            }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void create(SDL_WindowFlags p_rendererTypeFlag);
            void captureEvents();
            
            void chronoUpdate();
            unsigned long long getDeltaTime();

            void capFPS() {
                if (SDL_WINDOW_OPENGL) { SDL_GL_SwapWindow(_window); }
                // ===> if (1000/fps > frameStop - frameStart) SDL_Delay(1000/fps - (frameStop - frameStart));
            }
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
            SDL_WindowFlags _rendererType;
            bool _vSync = false;
            int targetFPS = 120;
            unsigned long long _time = 0;

            // screen refresh rate (warning plusieur écrans)
            // add audio => liste de piste preload au lancerment de la window + celles qui vont s'add ==> use set

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            bool _captureEvent(const SDL_Event& p_event);
            void _switchFullScreen2Maximized();
            void _takeScreenShot();
            void _dispose();
        };
    }
}
#endif
