#include "renderer_ogl.hpp"

#include "mesh_ogl.hpp"
#include "texture_ogl.hpp"
#include "pass/geometry_pass_ogl.hpp"
#include "pass/shading_pass_ogl.hpp"
#include "pass/final_pass_ogl.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "scene/objects/meshes/mesh.hpp"

#include "GL/gl3w.h"

#include <iostream>

namespace M3D
{
namespace Renderer
{
	RendererOGL::RendererOGL(){}

	RendererOGL::~RendererOGL() {
		delete _geometryPass;
		delete _shadingPass;
		delete _finalPass;
		for (std::pair<Scene::Mesh*, MeshOGL*> pair : _meshes) delete pair.second;
		for (std::pair<Image*, TextureOGL*> pair : _textures) delete pair.second;
	}

	void RendererOGL::init(SDL_Window* p_window) {
		// ****** go dans SDL ******
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GLContext glContext = SDL_GL_CreateContext(p_window);
		if (glContext == nullptr) throw std::exception(SDL_GetError());
		SDL_GL_MakeCurrent(p_window, glContext);
		// *************************

		if (gl3wInit()) throw std::exception("gl3wInit() failed");
		if (!gl3wIsSupported(4, 5)) throw std::exception("OpenGL version not supported");

		_geometryPass = new GeometryPassOGL("src/renderer/OpenGL/shaders/geometryPass.vert", "src/renderer/OpenGL/shaders/geometryPass.frag");
		_shadingPass = new ShadingPassOGL("src/renderer/OpenGL/shaders/shadingPass.vert", "src/renderer/OpenGL/shaders/shadingPass.frag");
		_finalPass = new FinalPassOGL("src/renderer/OpenGL/shaders/finalPass.vert", "src/renderer/OpenGL/shaders/finalPass.frag");

		resize(Application::getInstance().getWidth(), Application::getInstance().getHeight());

		glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.a);
	}

	void RendererOGL::resize(const int p_width, const int p_height) {
		_geometryPass->resize(p_width, p_height);
		_shadingPass->resize(p_width, p_height);
		_finalPass->resize(p_width, p_height);
	}

	void RendererOGL::drawFrame(SDL_Window* p_window) {
		_geometryPass->execute(Application::getInstance().getWidth(), Application::getInstance().getHeight(),_meshes);
		_shadingPass->execute(Application::getInstance().getWidth(), Application::getInstance().getHeight(),_geometryPass->getPositionMetalnessMap(),_geometryPass->getNormalRoughnessMap(),_geometryPass->getAlbedoMap());
		_finalPass->execute(Application::getInstance().getWidth(), Application::getInstance().getHeight(),_gamma,_shadingPass->getShadingMap());
	}

	void RendererOGL::createMesh(Scene::Mesh* p_mesh) { _meshes.insert(std::pair<Scene::Mesh*, MeshOGL*>(p_mesh, new MeshOGL(p_mesh))); }
	void RendererOGL::createTexture(Image* p_texture) { _textures.insert(std::pair<Image*, TextureOGL*>(p_texture, new TextureOGL(p_texture))); }
	void RendererOGL::deleteMesh(Scene::Mesh* p_mesh) { delete _meshes.at(p_mesh); _meshes.erase(_meshes.find(p_mesh)); }
	void RendererOGL::deleteTexture(Image* p_texture) { delete _textures.at(p_texture); _textures.erase(_textures.find(p_texture)); }
}
}