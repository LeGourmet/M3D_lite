#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "GL/gl3w.h"

#include "object_ogl.hpp"
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
			RendererOGL() {
				//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
				SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
			}
			~RendererOGL() {
				//_scene.cleanGL();
				glDeleteProgram(_program);
			}

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void init(SDL_Window* p_window) override {
				if( SDL_GL_CreateContext(p_window) == nullptr ) throw std::exception(SDL_GetError());
				updateVSync(_vSync);
				if (gl3wInit()) throw std::exception("gl3wInit() failed");
				//if (!gl3wIsSupported(4, 5)) throw std::exception("OpenGL version not supported");

				glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.a);
				glEnable(GL_DEPTH_TEST);

				_program = _initProgram("src/renderer/OpenGL/shaders/pass0.vert", "src/renderer/OpenGL/shaders/pass0.frag");
				_uMVPMatrixLoc = glGetUniformLocation(_program, "uMVPMatrix");
				_uCamPosLoc = glGetUniformLocation(_program, "uCamPos");

				glUseProgram(_program);
			}
			void resize(const int p_width, const int p_height) { glViewport(0, 0, p_width, p_height); };
			void drawFrame(SDL_Window* p_window) override;
			
			void createMesh(Scene::MeshTriangle* p_mesh) override { _objects.insert(std::pair<Scene::MeshTriangle*, Object_OGL>(p_mesh, Object_OGL())); }
			void createAmbiantMap(std::string p_path, Scene::MeshTriangle* p_mesh) override { _objects.at(p_mesh).generateAmbiantMap(p_path); }
			void createDiffuseMap(std::string p_path, Scene::MeshTriangle* p_mesh) override { _objects.at(p_mesh).generateDiffuseMap(p_path); }
			void createSpecularMap(std::string p_path, Scene::MeshTriangle* p_mesh) override { _objects.at(p_mesh).generateSpecularMap(p_path); }
			void createShininessMap(std::string p_path, Scene::MeshTriangle* p_mesh) override { _objects.at(p_mesh).generateShininessMap(p_path); }
			void createNormalMap(std::string p_path, Scene::MeshTriangle* p_mesh) override { _objects.at(p_mesh).generateNormalMap(p_path); }
			void createVAO(const std::vector<Vertex> p_vertices, const std::vector<unsigned int> p_indices, Scene::MeshTriangle* p_mesh) override { _objects.at(p_mesh).generateVAO(p_vertices, p_indices); }
			void deleteMesh(Scene::MeshTriangle* p_mesh) override { _objects.erase(_objects.find(p_mesh)); }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _program = GL_INVALID_INDEX;
			GLint  _uMVPMatrixLoc = GL_INVALID_INDEX;
			GLint  _uCamPosLoc = GL_INVALID_INDEX;

			std::map<Scene::MeshTriangle*,Object_OGL> _objects;

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			std::string _readShader(std::string p_path);
			void _readCompileShader(GLuint p_shader, std::string p_path);
			GLuint _initProgram(std::string p_pathVert, std::string p_pathFrag);
		};
	}
}
#endif