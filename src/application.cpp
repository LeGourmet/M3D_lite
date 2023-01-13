#include "application.hpp"

#include "controller/freefly_camera.hpp"
#include "engine/engine.hpp"
#include "model/scene.hpp"
#include "ui/user_interface.hpp"

namespace M3D
{
	Application::Application() : _width(1280), _height(720), _title("My M3D"), _running(false) {}

	Application::~Application()
	{
		delete _ui;
		delete _scene;
		delete _camera;
		delete _renderer;
	}

	void Application::start()
	{
		_running = true;

		_ui = new UI::UserInterface();
		_scene = new Scene::Scene();
		_camera = new Controller::FreeflyCamera();
		_renderer = new Engine::Engine();

		_camera->setScreenSize(_width, _height);
		_renderer->initRenderer(_ui->getWindow());

		while (_running)
			_update();
	}

	void Application::stop() { _running = false; }

	void Application::_update() const
	{
		const float deltaTime =  ui.getDeltaTime();

		_camera->update(deltaTime);
		_ui->update();
		_renderer->drawFrame(deltaTime);
	}
} // namespace M3D
