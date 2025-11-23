#include "window.hpp"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "user_interface/graphical_user_interface.hpp"

namespace M3D
{
	Window::Window() {
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
			throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		_window = SDL_CreateWindow(
			Application::getInstance().getTitle().c_str(),
			Application::getInstance().getWidth(),
			Application::getInstance().getHeight(),
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

		_glContext = SDL_GL_CreateContext(_window);
		if (_glContext == nullptr)
			throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

		SDL_GL_MakeCurrent(_window, _glContext);

		if (_window == nullptr)
			throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

		setVSync(_vSync);
	}

	Window::~Window() {
		if (_glContext) SDL_GL_DestroyContext(_glContext);
		if (_window) SDL_DestroyWindow(_window);
		SDL_Quit();
	}

	void Window::setVSync(bool p_vSync) {
		_vSync = p_vSync;
		SDL_GL_SetSwapInterval(p_vSync);
	}

	void Window::toggleVSync() {
		setVSync(!_vSync);
	}

	void Window::captureEvents() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			case SDL_EVENT_QUIT:
				Application::getInstance().stop();
				break;
			case SDL_EVENT_WINDOW_MINIMIZED:
				Application::getInstance().pause();
				do { SDL_WaitEvent(&event); } while (!(event.type == SDL_EVENT_WINDOW_RESTORED));
				Application::getInstance().resume();
				break;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				Application::getInstance().resize(event.window.data1, event.window.data2);
				break;
			case SDL_EVENT_WINDOW_FOCUS_LOST:
				Application::getInstance().pause();
				do { SDL_WaitEvent(&event); } while (!(event.type == SDL_EVENT_WINDOW_FOCUS_GAINED));
				Application::getInstance().resume();
				break;
			default:
				if (Application::getInstance().getGraphicalUserInterface().captureEvent(event)) { Application::getInstance().getSceneManager().clearEvents(); break; }
				Application::getInstance().getSceneManager().captureEvent(event); break;
			}
		}
	}

	void Window::switchFullScreenToMaximized() {
		Application::getInstance().pause();
		if (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN) {
			SDL_SetWindowFullscreen(_window, false);
			SDL_MaximizeWindow(_window);
		}
		else SDL_SetWindowFullscreen(_window, true);
		Application::getInstance().resume();
	}

	void Window::swapBuffers() { SDL_GL_SwapWindow(_window); }
}