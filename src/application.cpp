#include "application.hpp"

#include "input_output/window.hpp"
#include "user_interface/graphical_user_interface.hpp"
#include "scene/scene_manager.hpp"
#include "renderer/renderer_manager.hpp"

namespace M3D
{
	Application::Application() : _width(1280), _height(720), _title("My M3D"), _running(false) {}

	Application::~Application()
	{
		delete _window;
		delete _gui;
		delete _sceneManager;
		delete _rendererManager;
	}

	void Application::start()
	{
		_running = true;

		_window			 = new InputOutput::Window();
		_gui			 = new UserInterface::GraphicalUserInterface();
		_sceneManager	 = new Scene::SceneManager();
		_rendererManager = new Renderer::RendererManager();

		_window->create(_rendererManager->getRendererWindowFlags());
		_rendererManager->getRenderer().init(_window->get());
		_sceneManager->getCamera().setScreenSize(_width, _height);
		_sceneManager->getCamera().setPosition(Vec3f(0., 0., -1.));
		_gui->init();

		while (_running) _update();
	}

	void Application::stop() { _running = false; }

	void Application::resize(int p_width, int p_height) {
		_sceneManager->getCamera().setScreenSize(p_width,p_height);
		_rendererManager->getRenderer().resize(p_width,p_height);
	}

	void Application::_update() const
	{
		const float deltaTime = 0.1f; //getDeltaTime(); const float deltaTime = ImGui::GetIO().DeltaTime;

		_window->captureEvents();
		_sceneManager->update(deltaTime);
		_rendererManager->getRenderer().drawFrame(_window->get());
	}
}
