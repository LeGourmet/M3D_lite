#pragma once

#include <SDL3/SDL_video.h>

namespace M3D
{
	class Window
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Window();
		~Window();

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline SDL_Window& get() const { return *_window; }
		inline bool isVSync() const { return _vSync; }
		inline bool isFullscreen() const { return (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN); }
		inline SDL_GLContext getGLContext() const { return _glContext; }

		// ------------------------------------------------------ SETTERS ------------------------------------------------------
		void setVSync(bool p_vSync);

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void captureEvents();
		void switchFullScreenToMaximized();
		void toggleVSync();
		void swapBuffers();

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		SDL_Window* _window = nullptr;
		SDL_GLContext _glContext = nullptr;

		bool _vSync = true;
	};
}
