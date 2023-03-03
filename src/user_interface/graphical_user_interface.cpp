#include "graphical_user_interface.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "scene/lights/directional_light.hpp"
#include "scene/lights/point_light.hpp"

namespace M3D
{
	namespace UserInterface
	{
		GraphicalUserInterface::GraphicalUserInterface() { }
		GraphicalUserInterface::~GraphicalUserInterface() { }

		void GraphicalUserInterface::init(){
			
			//Application::getInstance().getSceneManager().addMeshes("assets/bunny2/bunny_2.obj");
			Application::getInstance().getSceneManager().addMeshes("assets/sponza/sponza.obj");
			//Application::getInstance().getSceneManager().addMeshes("assets/MyBlenderScene.obj"); 
			//Application::getInstance().getSceneManager().addMeshes("assets/Unity/UnityExporterScene.obj"); 
			//Application::getInstance().getSceneManager().addMeshes("assets/Teapot.obj");
			//Application::getInstance().getSceneManager().addMeshes("assets/Bunny.obj");
			//Application::getInstance().getSceneManager().addMeshes("assets/conference/conference.obj");

			Application::getInstance().getSceneManager().getCamera().setPosition(Vec3f(10., 20., 10.));
			Application::getInstance().getSceneManager().getCamera().rotate(Vec3f(0.5, -2.2, 0.));

			Application::getInstance().getSceneManager().addLight(new DirectionalLight(Vec3f(1., 1., 1.), 10., normalize(Vec3f(-0.25, -0.5, -0.25))));
			//Application::getInstance().getSceneManager().addLight(new DirectionalLight(Vec3f(0.9, 0.8, 0.1), 0.5, normalize(Vec3f(-0.25,-0.5,-0.25))));
			Application::getInstance().getSceneManager().addLight(new PointLight(Vec3f(0., 20, 0.), Vec3f(0., 0., 1.), 10000., 500.));
		}

		bool GraphicalUserInterface::captureEvent(SDL_Event event) {
			return false; 
		}

		void GraphicalUserInterface::update(unsigned long long p_deltaTime) {

		}
	}
}