#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "GL/gl3w.h"
#include "SDL.h"
#include "camera/baseCamera.hpp"
#include "camera/freeflyCamera.hpp"
#include "camera/trackballCamera.hpp"
#include "geometry/triangle_mesh_model.hpp"
#include "utils/define.hpp"
#include <map>
#include <vector>

namespace M3D_ISICG
{
	class Scene
	{
	  public:
		Scene() {
			_camera = new FreeflyCamera();
			_freefly = true;
		}

		~Scene() {
			_scene.cleanGL();
			glDeleteProgram(_program);
		}

		bool init();
		void animate(const float p_deltaTime);
		void handleEvents(const SDL_Event& p_event);
		void render();
		void displayUI();
		void resize(const int p_width, const int p_height);

		int getWindowWidth() const { return _windowWidth; }
		int getWindowHeight() const { return _windowHeight; }


	private:
		bool _initProgram();
		void _initCamera();

	private:
		// ================ Scene data.
		TriangleMeshModel _scene;
		BaseCamera*		  _camera = nullptr;
		bool			  _freefly;
		// ================

		// ================ GL data.
		GLuint _program = GL_INVALID_INDEX;
		GLint  _uMVPMatrixLoc = GL_INVALID_INDEX;
		GLint  _uMMatrix = GL_INVALID_INDEX;
		GLint  _uCamPosLoc = GL_INVALID_INDEX;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f(0.8f, 0.8f, 0.8f, 1.f); // Background color
		// Camera
		float _cameraSpeed = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _fovy = 60.f;
		// ================

		static const std::string _shaderFolder; // define .hpp ?
		int _windowWidth = 0;
		int _windowHeight = 0;
	};
} // namespace M3D_ISICG

#endif // __SCENE_HPP__
