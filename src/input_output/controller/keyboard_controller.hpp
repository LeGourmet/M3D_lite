#pragma once

#include <SDL3/SDL_events.h>

#include <set>

namespace M3D
{
	class KeyboardController
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		KeyboardController() {}
		~KeyboardController() {}

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void receiveEvent(const SDL_Event& p_event) {
			if (p_event.type == SDL_EVENT_KEY_DOWN && !p_event.key.repeat) _pressedButtons.insert(p_event.key.scancode);
			if (p_event.type == SDL_EVENT_KEY_UP) _pressedButtons.erase(p_event.key.scancode);
		}

		void clearEvent(const SDL_Scancode& p_key) { _pressedButtons.erase(p_key); }
		void clearEvents() { _pressedButtons.clear(); }

	protected:
		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		bool _isKeyPressed(const SDL_Scancode& p_key) { return _pressedButtons.find(p_key) != _pressedButtons.end(); }

	protected:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		std::set<SDL_Scancode> _pressedButtons = std::set<SDL_Scancode>();
	};
}
