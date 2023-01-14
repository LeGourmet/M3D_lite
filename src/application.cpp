#include "application.hpp"

#include "user_interface/window.hpp"
#include "user_interface/input_manager.hpp"
#include "user_interface/audio_manager.hpp"
#include "user_interface/graphical_user_interface.hpp"
#include "scene/scene_manager.hpp"
#include "renderer/renderer_engine.hpp"

namespace M3D
{
	Application::Application() : _width(1280), _height(720), _title("My M3D"), _running(false) {}

	Application::~Application()
	{
		delete _window;
		delete _inputManager;
		delete _audioManager;
		delete _gui;
		delete _sceneManager;
		delete _renderer;
	}

	void Application::start()
	{
		_running = true;

		_window			= new UserInterface::Window();
		_inputManager	= new UserInterface::InputManager();
		_audioManager	= new UserInterface::AudioManager();
		_gui			= new UserInterface::GraphicalUserInterface();
		_sceneManager	= new Scene::SceneManager();
		_renderer		= new Renderer::RenderEngine();

		_renderer->init(_window->get());

		while (_running)
			_update();
	}

	void Application::stop() { _running = false; }

	void Application::_update() const
	{
		const float deltaTime = 0.1f;//getDeltaTime();

		_inputManager->captureEvents();
		//_audioManager->update(deltaTime);
		//_gui->update(deltaTime);
		//_sceneManager->update(deltaTime);
		_renderer->drawFrame(deltaTime);
	}
}
