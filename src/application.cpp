#include "application.hpp"

#include "input_output/window.hpp"
#include "user_interface/graphical_user_interface.hpp"
#include "scene/scene_manager.hpp"
#include "renderer/renderer.hpp"
#include "renderer/OpenGL/renderer_ogl.hpp"

namespace M3D
{
	Application::Application() : _width(1280), _height(720), _title("My M3D"), _running(false) {}

	Application::~Application()
	{
		delete _window;
		delete _gui;
		delete _sceneManager;
		delete _renderer;
	}

	void Application::start()
	{
		_running = true;
		
		_window			 = new InputOutput::Window();
		_renderer		 = new Renderer::RendererOGL();
		_sceneManager	 = new Scene::SceneManager();
		_gui			 = new UserInterface::GraphicalUserInterface();

		_window->create(_renderer->getWindowFlag());
		_renderer->init(_window->get());
		_renderer->resize(_width, _height);
		_sceneManager->getCamera().setScreenSize(_width, _height);
		_gui->init();

		_window->chronoUpdate();
		while (_running) _update();
	}

	void Application::stop() { _running = false; }

	void Application::pause() {
		// ui pause => (set screen pause / do nothing if scene isn't display)
		// clear events 
	}
	
	void Application::resume() { _window->chronoUpdate(); }

	void Application::resize(int p_width, int p_height) {
		_width = p_width;
		_height = p_height;
		_sceneManager->getCamera().setScreenSize(p_width,p_height);
		_renderer->resize(p_width,p_height);
	}

	void Application::_update() const
	{
		unsigned long long deltaTime = _window->getDeltaTime();

		_window->captureEvents();
		_sceneManager->update(deltaTime);
		_gui->update(deltaTime);
		_renderer->drawFrame(_window->get());

		//_window->brideFPS();
		// ===> if (1000/fps > frameStop - frameStart) SDL_Delay(1000/fps - (frameStop - frameStart));
	}
}
