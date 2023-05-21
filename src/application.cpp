#include "application.hpp"

#include "input_output/window.hpp"
#include "user_interface/graphical_user_interface.hpp"
#include "scene/scene_manager.hpp"
#include "renderer/OpenGL/renderer_ogl.hpp"

namespace M3D
{
	Application::Application() : _width(1280), _height(720), _title("My M3D"), _running(false) {}

	Application::~Application()
	{
		delete _sceneManager;
		delete _gui;
		delete _renderer;
		delete _window;
	}

	void Application::start()
	{
		_running = true;
		
		// decide if use ogl or vulkan and if we are on / linux / mac / android / windows
		_window			 = new InputOutput::Window(SDL_WINDOW_OPENGL);
		_renderer		 = new Renderer::RendererOGL(&_window->get());
		_sceneManager	 = new Scene::SceneManager(_width, _height);
		_gui			 = new UserInterface::GraphicalUserInterface(&_window->get(),_window->getGLContext());

		_window->chronoUpdate();
		while (_running) _update();
	}

	void Application::stop() { _running = false; }

	void Application::pause() {
		_sceneManager->clearEvents();
		_gui->clearEvents();
		_gui->pause();
	}
	
	void Application::resume() { _window->chronoUpdate(); }

	void Application::resize(int p_width, int p_height) {
		_width = p_width;
		_height = p_height;
		_sceneManager->resize(p_width,p_height);
		_renderer->resize(p_width,p_height);
		//_gui->resize(p_width, p_height);
	}

	void Application::_update() const {
		unsigned long long deltaTime = _window->getDeltaTime();

		_window->captureEvents();
		_gui->update(deltaTime);
		_sceneManager->update(deltaTime);

		_renderer->drawFrame();			// &_window->get() / context ?
		_gui->drawFrame();				// &_window->get() / context ?

		_window->capFPS(deltaTime);
	}
}
