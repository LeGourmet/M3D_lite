#include "application.hpp"

#include "input_output/window.hpp"
#include "user_interface/graphical_user_interface.hpp"
#include "scene/scene_manager.hpp"
#include "renderer/renderer.hpp"

namespace M3D
{
	Application::Application() : _width(1280), _height(720), _title("M3D"), _running(false) {}

	Application::~Application() {
		delete _sceneManager;
		delete _gui;
		delete _renderer;
		delete _window;
	}

	void Application::start() {
		_running		= true;
		
		_window			= new Window();
		_renderer		= new Renderer(&_window->get());
		_sceneManager	= new SceneManager(_width, _height);
		_gui			= new GraphicalUserInterface(&_window->get(),_window->getGLContext());

		_chrono.start();
		while (_running) _update();
	}

	void Application::stop() { _running = false; }

	void Application::pause() {
		_sceneManager->clearEvents();
		//_sceneManager->pause();
		_gui->clearEvents();
		//_gui->pause();
	}
	
	void Application::resume() { _chrono.start(); }

	void Application::resize(uint p_width, uint p_height) {
		_width = p_width;
		_height = p_height;
		_sceneManager->resize(p_width,p_height);
		_renderer->resize(p_width,p_height);
		_gui->resize(p_width, p_height);
	}

	void Application::_update() {
		_chrono.stop();
		float deltaTime = _chrono.elapsedTime();
		_chrono.start();

		_window->captureEvents();
		_gui->update(deltaTime);
		_sceneManager->update(deltaTime);

		_renderer->drawFrame();			// &_window->get() / context ?
		_gui->drawFrame();				// &_window->get() / context ?
	
		_window->swapBuffers();
	}
}
