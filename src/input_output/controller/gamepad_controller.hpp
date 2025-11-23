#pragma once

#include <SDL3/SDL_events.h>

namespace M3D
{
	class GamepadController
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		GamepadController() {}
		~GamepadController() {}

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void receiveEvent(const SDL_Event& p_event) {}

		void clearEvents() {}

	};
}
