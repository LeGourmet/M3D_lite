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
            Window(SDL_WindowFlags p_rendererTypeFlag);
            ~Window();

            // ----------------------------------------------------- GETTERS -------------------------------------------------------
            inline SDL_Window& get() const { return *_window; }
            inline bool getVSync() { return _vSync; }

            // ----------------------------------------------------- SETTERS -------------------------------------------------------
            void setVSync(bool p_vSync);

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void captureEvents();
            
            void chronoUpdate();
            unsigned long long getDeltaTime();

            void capFPS();
            // add loader d'audio 
            // add fonction qui permettent de supprimer des pistes
            // add fonction qui lis une piste / qui stop 

        private:
            // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
            SDL_Window* _window = nullptr;
            SDL_WindowFlags _rendererType;
            
            // add audio => liste de piste preload au lancerment de la window + celles qui vont s'add ==> use set
            unsigned long long _time = 0;
            
            bool _vSync = true;
            int targetFPS = 120;

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            bool _captureEvent(const SDL_Event& p_event);
            void _switchFullScreen2Maximized();
            void _takeScreenShot();
            void _dispose();
        };
    }
}
#endif
