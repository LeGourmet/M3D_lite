#pragma once

#include "utils/defines.hpp"
#include <SDL3/SDL_events.h>

#include "imgui.h"

namespace M3D
{
	class GraphicalUserInterface
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		GraphicalUserInterface(SDL_Window* p_window, const SDL_GLContext& p_glContext);
		~GraphicalUserInterface();

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void pause();
		void resize(const uint p_width, const uint p_height);
		void update(float p_deltaTime);
		void drawFrame();

		bool captureEvent(const SDL_Event& p_event);
		void clearEvents();

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		bool _hide = false;

		ImFont* _font;
	};
}
