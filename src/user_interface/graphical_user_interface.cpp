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
		//Application::getInstance().getSceneManager().loadNewScene("assets/sponza/Sponza.gltf");
		//Application::getInstance().getSceneManager().loadNewScene("assets/musee/musee.glb");
		Application::getInstance().getSceneManager().loadNewScene("assets/new_sponza/main/NewSponza_Main_glTF_002.gltf");
		//Application::getInstance().getSceneManager().addAsset("assets/new_sponza/curtains/NewSponza_Curtains_glTF.gltf");
		//Application::getInstance().getSceneManager().addAsset("assets/new_sponza/ivy/NewSponza_IvyGrowth_glTF.gltf");
		//Application::getInstance().getSceneManager().addAsset("assets/new_sponza/trees/NewSponza_CypressTree_glTF.gltf");
	}

	bool GraphicalUserInterface::captureEvent(SDL_Event event) {
		return false; 
	}

	void GraphicalUserInterface::update(unsigned long long p_deltaTime) {

	}
}
}