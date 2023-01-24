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
            /* void _screenshot() {
			    try{
					SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(flippedPixels, _width, _height, 32, _width * 4,
						0x000000FF,
						0x0000FF00,
						0x00FF0000,
						0xFF000000
					);
					IMG_savePNG( surface, "./screenshot/screenTest.png" );
				}
				catch (const std::exception& e){
					std::cerr << "Cannot save screenshot: " << e.what() << std::endl;
				}
			}*/

        private:
            // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
            SDL_Window* _window = nullptr;
            // add audio => liste de piste preload au lancerment de la window + celles qui vont s'add ==> use set
            // add shortcut => capture event prioritaire par rapport à scene et gui
                // => imprim-ecran / ... 

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void _dispose();
        };
    }
}
#endif
