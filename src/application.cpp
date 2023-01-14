#include "application.hpp"

#include "user_interface/window.hpp"
#include "user_interface/user_interface.hpp"
#include "controller/freefly_camera.hpp"
#include "scene/scene.hpp"
#include "engine/engine.hpp"

namespace M3D
{
	Application::Application() : _width(1280), _height(720), _title("My M3D"), _running(false) {}

	Application::~Application()
	{
		delete _window;
		delete _ui;
		delete _scene;
		delete _camera;
		delete _renderer;
	}

	void Application::start()
	{
		_running = true;

		_window = new UserInterface::Window();
		_ui = new UserInterface::UserInterface();
		_camera = new Controller::FreeflyCamera();
		_scene = new Scene::Scene();
		_renderer = new Engine::Engine();

		_camera->setScreenSize(_width,_height);
		_renderer->initRenderer(_window->get());

		while (_running)
			_update();
	}

	void Application::stop() { _running = false; }

	void Application::_update() const
	{
		const float deltaTime = _ui->getDeltaTime();

		_camera->update(deltaTime);
		_window->captureEvents();
		_renderer->drawFrame(deltaTime);
	}
}
