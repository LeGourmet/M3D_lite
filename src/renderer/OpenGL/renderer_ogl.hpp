#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "GL/gl3w.h"

#include "texture_ogl.hpp"
#include "object_ogl.hpp"
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
				_program = _initProgram("./shaders/pass0.vert","./shaders/pass0.frag");
				_uMVPMatrixLoc = glGetUniformLocation(_program, "uMVPMatrix");
				_uCamPosLoc = glGetUniformLocation(_program, "uCamPos");
			}
			void resize(const int p_width, const int p_height) { glViewport(0, 0, p_width, p_height); };
			void drawFrame() override;
			
			Texture* createTexture(const std::string p_path) override { return new TextureOGL(p_path); }
			Object* createObject(const std::vector<Vertex> p_vertices, const std::vector<unsigned int> p_indices) override { return new ObjectOGL(p_vertices,p_indices); }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _program = GL_INVALID_INDEX;
			GLint  _uMVPMatrixLoc = GL_INVALID_INDEX;
			GLint  _uCamPosLoc = GL_INVALID_INDEX;

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			const GLchar* _readShader(std::string p_path);
			void _readCompileShader(GLuint p_shader, std::string p_path);
			GLuint _initProgram(std::string p_pathVert, std::string p_pathFrag);
		};
	}
}
#endif