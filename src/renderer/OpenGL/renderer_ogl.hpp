#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "GL/gl3w.h"

#include "texture_ogl.hpp"
#include "vao_ogl.hpp"
#include "renderer/renderer.hpp"

namespace M3D
{
	namespace Renderer
	{
		class RendererOGL : public Renderer
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			RendererOGL() {}
			~RendererOGL() {}

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void init(SDL_Window* p_window) override {
				if( SDL_GL_CreateContext(p_window) == nullptr ) throw std::exception(SDL_GetError());
				if (gl3wInit()) throw std::exception("gl3wInit() failed");
				//SDL_GL_SetSwapInterval(true);

				_program = _initProgram("C:/Users/babow/Desktop/M3D_lite/src/renderer/OpenGL/shaders/pass0.vert","C:/Users/babow/Desktop/M3D_lite/src/renderer/OpenGL/shaders/pass0.frag");
				_uMVPMatrixLoc = glGetUniformLocation(_program, "uMVPMatrix");
				_uCamPosLoc = glGetUniformLocation(_program, "uCamPos");
			}
			void resize(const int p_width, const int p_height) { glViewport(0, 0, p_width, p_height); };
			void drawFrame(SDL_Window* p_window) override;
			
			unsigned int createTexture(const std::string p_path) override;
			unsigned int createVAO(const std::vector<Vertex> p_vertices, const std::vector<unsigned int> p_indices) override;
			void deleteTexture(unsigned int p_id) override;
			void deleteVAO(unsigned int p_id) override;

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _program = GL_INVALID_INDEX;
			GLint  _uMVPMatrixLoc = GL_INVALID_INDEX;
			GLint  _uCamPosLoc = GL_INVALID_INDEX;

			// TODO convert to set
			std::vector<VAO_OGL*> _VAOs = std::vector<VAO_OGL*>();
			std::vector<Texture_OGL*> _textures = std::vector<Texture_OGL*>();

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			const GLchar* _readShader(std::string p_path);
			void _readCompileShader(GLuint p_shader, std::string p_path);
			GLuint _initProgram(std::string p_pathVert, std::string p_pathFrag);
		};
	}
}
#endif