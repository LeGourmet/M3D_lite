#include "graphical_user_interface.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"

namespace M3D
{
namespace UserInterface
{
	GraphicalUserInterface::GraphicalUserInterface() { 
		//Application::getInstance().getSceneManager().loadNewScene("assets/musee.gltf");
		//Application::getInstance().getSceneManager().loadNewScene("assets/alphaTest.glb");
		//Application::getInstance().getSceneManager().loadNewScene("assets/shadow.glb");
		//Application::getInstance().getSceneManager().loadNewScene("assets/chest.glb");
		//Application::getInstance().getSceneManager().loadNewScene("assets/toyCar.glb");
		Application::getInstance().getSceneManager().loadNewScene("assets/sponza.glb");
		//Application::getInstance().getSceneManager().loadNewScene("assets/emissive.glb");
	}

	GraphicalUserInterface::~GraphicalUserInterface() { }

	void GraphicalUserInterface::pause() { }
	
	void GraphicalUserInterface::update(unsigned long long p_deltaTime) { }

	void GraphicalUserInterface::drawFrame() { }

	bool GraphicalUserInterface::captureEvent(const SDL_Event& event) {
		return false; 
	}

	void GraphicalUserInterface::clearEvents() {
		//(set screen pause / do nothing if scene isn't display)
	}
}
}