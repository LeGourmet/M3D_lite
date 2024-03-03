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
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Window(Uint32 p_rendererTypeFlag);
            ~Window();

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            inline SDL_Window& get() const { return *_window; }
            inline unsigned long long getTime() { return _time; }
            inline bool getVSync() { return _vSync; }
            inline unsigned int getTargetFps() { return _targetFPS; }

            inline SDL_GLContext getGLContext() { return _glContext; }// todo virer !


            // ------------------------------------------------------ SETTERS ------------------------------------------------------
            void setVSync(bool p_vSync);
            void setTargetFPS(unsigned int p_targetFPS);

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void captureEvents();
            
            void chronoUpdate();
            unsigned long long getDeltaTime();

            void capFPS(unsigned long long p_elapsedTime);
            // add loader d'audio 
            // add fonction qui permettent de supprimer des pistes
            // add fonction qui lis une piste / qui stop 
            // add swap renderer type

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            SDL_Window* _window = nullptr;
            Uint32 _rendererType;
            SDL_GLContext _glContext = nullptr; // todo virer pas bien !!
            
            // add audio => liste de piste preload au lancerment de la window + celles qui vont s'add ==> use set
            // todo use cycle CPU pour fps et time plus précis
            unsigned long long _time = 0;
            
            bool _vSync = true;
            unsigned int _targetFPS = 120;

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            bool _captureEvent(const SDL_Event& p_event);
            void _switchFullScreenToMaximized();
            void _takeScreenShot();
            void _dispose();
        };
    }
}

#endif
