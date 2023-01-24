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
			Application::getInstance().getSceneManager().addMeshes("assets/MyBlenderScene.obj"); 
			//Application::getInstance().getSceneManager().addMeshes("assets/Teapot.obj");
			//Application::getInstance().getSceneManager().addMeshes("assets/Bunny.obj");
		}

		bool GraphicalUserInterface::captureEvent(SDL_Event event) {
			return false; 
		}
	}
}