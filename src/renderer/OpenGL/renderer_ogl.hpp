#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "GL/gl3w.h"

#include "mesh_ogl.hpp"
#include "pass/geometry_pass_ogl.hpp"
#include "pass/shading_pass_ogl.hpp"
#include "pass/final_pass_ogl.hpp"
#include "renderer/renderer.hpp"

#include <map>

namespace M3D
{
	namespace Renderer
	{
		class RendererOGL : public Renderer
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			RendererOGL() {	}
			~RendererOGL() {
				delete _geometryPass;
				delete _shadingPass;
				delete _finalPass;
				for (std::pair<Scene::Mesh*, MeshOGL*> pair : _meshes) delete pair.second;
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			SDL_WindowFlags getWindowFlag() { return SDL_WINDOW_OPENGL; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void init(SDL_Window* p_window) override {
				SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
				SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
				SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

				SDL_GLContext glContext = SDL_GL_CreateContext(p_window);
				if( glContext == nullptr ) throw std::exception(SDL_GetError());
				SDL_GL_MakeCurrent(p_window, glContext);
				setVSync(_vSync);
				if (gl3wInit()) throw std::exception("gl3wInit() failed");
				if (!gl3wIsSupported(4, 5)) throw std::exception("OpenGL version not supported");

				_geometryPass	= new GeometryPassOGL("src/renderer/OpenGL/shaders/geometryPass.vert", "src/renderer/OpenGL/shaders/geometryPass.frag");
				_shadingPass	= new ShadingPassOGL("src/renderer/OpenGL/shaders/shadingPass.vert", "src/renderer/OpenGL/shaders/shadingPass.frag",_geometryPass->getAmbientMap());
				_finalPass		= new FinalPassOGL("src/renderer/OpenGL/shaders/finalPass.vert", "src/renderer/OpenGL/shaders/finalPass.frag");
				
				SDL_GetWindowSize(p_window, &_viewport_width, &_viewport_height);
				resize(_viewport_width, _viewport_height);

				glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.a);
			}
			void setVSync(bool p_vSync) override { _vSync = p_vSync; SDL_GL_SetSwapInterval(p_vSync); }
			void resize(const int p_width, const int p_height)  override;
			void drawFrame(SDL_Window* p_window) override;
			
			void createMesh(Scene::Mesh* p_mesh) override { _meshes.insert(std::pair<Scene::Mesh*, MeshOGL*>(p_mesh, new MeshOGL(p_mesh))); }
			void deleteMesh(Scene::Mesh* p_mesh) override { delete _meshes.at(p_mesh); _meshes.erase(_meshes.find(p_mesh)); }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GeometryPassOGL* _geometryPass;
			ShadingPassOGL* _shadingPass;
			FinalPassOGL* _finalPass;

			int _viewport_width = 0;
			int _viewport_height = 0;
			std::map<Scene::Mesh*,MeshOGL*> _meshes;
		};
	}
}
#endif