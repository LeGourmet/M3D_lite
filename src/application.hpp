#pragma once

#include "utils/defines.hpp"
#include "utils/chrono.hpp"

#include <string>

namespace M3D
{
	class Window;
	class GraphicalUserInterface;
	class SceneManager;
	class Renderer;

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

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline std::string getTitle() const { return _title; }
		inline uint getWidth() const { return _width; }
		inline uint getHeight() const { return _height; }
		inline bool isRunning() const { return _running; }
		
		inline Window& getWindow() const { return *_window; }
		inline GraphicalUserInterface& getGraphicalUserInterface() const { return *_gui; }
		inline SceneManager& getSceneManager() const { return *_sceneManager; }
		inline Renderer& getRenderer() const { return *_renderer; }

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void start();
		void stop();
		void pause();
		void resume();
		void resize(uint p_width, uint p_height);

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		std::string				_title;
		uint					_width;
		uint					_height;
		bool					_running;

		Window*					_window{};
		GraphicalUserInterface*	_gui{};
		SceneManager*			_sceneManager{};
		Renderer*				_renderer{};

		Chrono					_chrono;

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void _update();
	};
}
