#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <string>

namespace M3D
{
	namespace InputOutput { class Window; }
	namespace UserInterface { class GraphicalUserInterface; }
	namespace Scene { class SceneManager; }
	namespace Renderer { class RendererManager; }

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
		
		InputOutput::Window& getWindow() const { return *_window; }
		UserInterface::GraphicalUserInterface& getGraphicalUserInterface() const { return *_gui; }
		Scene::SceneManager& getSceneManager() const { return *_sceneManager; }
		Renderer::RendererManager& getRendererManager() const { return *_rendererManager; }

		// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
		void start();
		void stop();

		void resize(int p_width, int p_height);

		private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		std::string _title;
		int		    _width;
		int		    _height;
		bool		_running;

		InputOutput::Window*					_window{};
		UserInterface::GraphicalUserInterface*	_gui{};
		Scene::SceneManager*					_sceneManager{};
		Renderer::RendererManager*				_rendererManager{};

		// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
		void _update() const;
	};
}
#endif