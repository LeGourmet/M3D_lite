#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <string>

namespace M3D
{
	namespace UserInterface { class Window; }
	namespace UserInterface { class UserInterface; }
	namespace Camera { class Camera; }
	namespace Scene { class Scene; }
	namespace Engine { class Engine; }

	class Application final
	{
		public:
		static Application& getInstance()
		{
			static Application instance;
			return instance;
		}
		
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		// ----------------------------------------------------- GETTERS -------------------------------------------------------
		std::string getTitle() { return Application::_title; }
		unsigned int getWidth() const { return Application::_width; }
		unsigned int getHeight() const { return Application::_height; }
		bool isRunning() const { return Application::_running; }
		
		UserInterface::Window& getWindow() const { return *_window; }
		UserInterface::UserInterface& getUserInterface() const { return *_ui; }
		Camera::Camera& getCamera() const { return *_camera; }
		Scene::Scene& getScene() const { return *_scene; }
		Engine::Engine& getRenderer() const { return *_renderer; }

		// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
		void start();
		void stop();

		private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		std::string _title;
		int		    _width;
		int		    _height;
		bool		_running;

		UserInterface::Window*			_window{};
		UserInterface::UserInterface*	_ui{};
		Camera::Camera*				_camera{};
		Scene::Scene*					_scene{};
		Engine::Engine*					_renderer{};

		// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
		void _update() const;
	};
}
#endif
