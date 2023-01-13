#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "scene/scene.hpp"
#include <string>

namespace M3D
{

	namespace UI { class UserInterface; }
	namespace Scene { class Scene; }
	namespace Engine { class Engine; }
	namespace Controller { class Camera; }

	class Application final
	{
		public:
		static Application& getInstance()
		{
			static Application instance;
			return instance;
		}
		
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		// ----------------------------------------------------- GETTERS -------------------------------------------------------
		std::string getTitle() { return Application::_title; }
		unsigned int getWidth() const { return Application::_width; }
		unsigned int getHeight() const { return Application::_height; }
		bool isRunning() const { return Application::_running; }
		
		UI::UserInterface& getUI() const { return *_ui; }
		Scene::Scene& getScene() const { return *_scene; }
		Controller::Camera& getCamera() const { return *_camera; }
		Engine::Engine& getRenderer() const { return *_renderer; }

		// ---------------------------------------------------- FONCTIONS -------------------------------------------------------
		void start();
		void stop();

		private:
		// ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
		std::string _title;
		int		    _width;
		int		    _height;
		bool		_running;

		UI::UserInterface*  _ui{};
		Scene::Scene*		_scene{};
		Controller::Camera* _camera{};
		Engine::Engine*	    _renderer{};

		// ----------------------------------------------------- FONCTIONS -------------------------------------------------------
		void _update() const;
	};
} // namespace M3D
#endif // __APPLICATION_HPP__
