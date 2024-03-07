#include "renderer_ogl.hpp"

#include "GL/gl3w.h"

#include "application.hpp"

#include <iostream>

namespace M3D
{
namespace Renderer
{
	RendererOGL::RendererOGL(SDL_Window* p_window) {
		if (gl3wInit()) throw std::exception("gl3wInit() failed");
		if (!gl3wIsSupported(4, 5)) throw std::exception("OpenGL version not supported");

		_stageGeometryOGL			= new StageGeometryOGL();
		_stageLightingOGL			= new StageLightingOGL();
		_stagePostProcessingOGL		= new StagePostProcessingOGL();

		resize(Application::getInstance().getWidth(), Application::getInstance().getHeight());
	}

	RendererOGL::~RendererOGL() {
		delete _stageGeometryOGL;
		delete _stageLightingOGL;
		delete _stagePostProcessingOGL;
		for (std::pair<Scene::Mesh*, MeshOGL*> pair : _meshes) delete pair.second;
		for (std::pair<Texture*, TextureOGL*> pair : _textures) delete pair.second;
	}

	void RendererOGL::resize(const int p_width, const int p_height) {
		_stageGeometryOGL->resize(p_width, p_height);
		_stageLightingOGL->resize(p_width, p_height);
		_stagePostProcessingOGL->resize(p_width, p_height);
	}

	void RendererOGL::drawFrame() {
		glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.a);
		_stageGeometryOGL->execute(Application::getInstance().getWidth(), Application::getInstance().getHeight(), _meshes, _textures);
		_stageLightingOGL->execute(Application::getInstance().getWidth(), Application::getInstance().getHeight(), _meshes, _textures, _stageGeometryOGL->getAlbedoMap(), _stageGeometryOGL->getNormalMap(), _stageGeometryOGL->getMetalnessRoughnessMap(), _stageGeometryOGL->getEmissiveMap(), _stageGeometryOGL->getDepthMap(), _stageGeometryOGL->getRootTransparency(), _stageGeometryOGL->getSSBOTransparency());
		_stagePostProcessingOGL->execute(Application::getInstance().getWidth(), Application::getInstance().getHeight(), _meshes, _textures, _stageLightingOGL->getLightingMap());

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (err)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			std::cout << error << std::endl;
		}
	}

	void RendererOGL::createMesh(Scene::Mesh* p_mesh) { _meshes.insert(std::pair<Scene::Mesh*, MeshOGL*>(p_mesh, new MeshOGL(p_mesh))); }
	void RendererOGL::createTexture(Texture* p_texture) { _textures.insert(std::pair<Texture*, TextureOGL*>(p_texture, new TextureOGL(p_texture))); }

	void RendererOGL::addInstanceMesh(Scene::Mesh* p_mesh, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) { _meshes.at(p_mesh)->addInstance(p_M_matrix, p_V_matrix, p_P_matrix); }
	void RendererOGL::updateInstanceMesh(Scene::Mesh* p_mesh, unsigned int p_id, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) { _meshes.at(p_mesh)->updateTransformMatrix(p_id, p_M_matrix, p_V_matrix, p_P_matrix); }

	void RendererOGL::deleteMesh(Scene::Mesh* p_mesh) { delete _meshes.at(p_mesh); _meshes.erase(_meshes.find(p_mesh)); }
	void RendererOGL::deleteTexture(Texture* p_texture) { delete _textures.at(p_texture); _textures.erase(_textures.find(p_texture)); }
}
}