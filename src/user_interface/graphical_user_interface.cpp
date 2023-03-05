#include "graphical_user_interface.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"

namespace M3D
{
namespace UserInterface
{
	GraphicalUserInterface::GraphicalUserInterface() { }
	GraphicalUserInterface::~GraphicalUserInterface() { }

	void GraphicalUserInterface::init(){
		Application::getInstance().getSceneManager().loadNewScene("assets/gltf/sponza/Sponza.gltf");
		//Application::getInstance().getSceneManager().loadNewScene("assets/gltf/musee/musee.glb");
	}

	bool GraphicalUserInterface::captureEvent(SDL_Event event) {
		return false; 
	}

	void GraphicalUserInterface::update(unsigned long long p_deltaTime) {

	}
}
}