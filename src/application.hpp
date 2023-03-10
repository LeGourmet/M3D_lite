#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <string>

namespace M3D
{
	namespace InputOutput { class Window; }
	namespace UserInterface { class GraphicalUserInterface; }
	namespace Scene { class SceneManager; }
	namespace Renderer { class Renderer; }

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
		inline std::string getTitle() const { return _title; }
		inline unsigned int getWidth() const { return _width; }
		inline unsigned int getHeight() const { return _height; }
		inline bool isRunning() const { return _running; }
		
		inline InputOutput::Window& getWindow() const { return *_window; }
		inline UserInterface::GraphicalUserInterface& getGraphicalUserInterface() const { return *_gui; }
		inline Scene::SceneManager& getSceneManager() const { return *_sceneManager; }
		inline Renderer::Renderer& getRenderer() const { return *_renderer; }

		// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
		void start();
		void stop();
		void pause();
		void resume();
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
		Renderer::Renderer*						_renderer{};

		// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
		void _update() const;
	};
}
#endif
