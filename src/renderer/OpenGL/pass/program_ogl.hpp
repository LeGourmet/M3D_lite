#ifndef __PROGRAM_OGL_HPP__
#define __PROGRAM_OGL_HPP__

#include "GL/gl3w.h"

#include <string>
#include <fstream>
#include <sstream>

namespace M3D
{
	namespace Renderer
	{
		class ProgramOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ProgramOGL(std::string p_pathCompute) {
				_program = glCreateProgram();
				
				GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
				_compileShader(computeShader, p_pathCompute);
				glAttachShader(_program, computeShader);

				glLinkProgram(_program);

				glDeleteShader(computeShader);
			}
			
			ProgramOGL(std::string p_pathVertex, std::string p_pathGeometry, std::string p_pathFragment) {
				_program = glCreateProgram();

 				GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
				_compileShader(vertexShader, p_pathVertex);
				glAttachShader(_program, vertexShader);

				GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
				if (std::filesystem::exists(p_pathGeometry)) {
					_compileShader(geometryShader, p_pathGeometry);
					glAttachShader(_program, geometryShader);
				}

				GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
				if (std::filesystem::exists(p_pathFragment)) {
					_compileShader(fragmentShader, p_pathFragment);
					glAttachShader(_program, fragmentShader);
				}

				glLinkProgram(_program);

				glDeleteShader(vertexShader);
				glDeleteShader(geometryShader);
				glDeleteShader(fragmentShader);
			}
			
			~ProgramOGL() { glDeleteProgram(_program); }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			virtual void resize(int p_width, int p_height) = 0;

		protected:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _program = GL_INVALID_INDEX;
			// TODO add map or vector for color / depth attachements
			// TODO add map<string,gluint> uniformLoc;

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void _generateMap(GLuint* p_texture) {
				glCreateTextures(GL_TEXTURE_2D, 1, p_texture);
				glTextureParameteri(*p_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			void _generateCubeMap(GLuint* p_texture) {
				glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, p_texture);
				glTextureParameteri(*p_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			
			void _generateAndAttachColorMap(GLuint p_framebuffer, GLuint* p_texture, int p_id) {
				_generateMap(p_texture);
				glNamedFramebufferTexture(p_framebuffer, GL_COLOR_ATTACHMENT0 + p_id, *p_texture, 0);
			}

			void _generateAndAttachDepthMap(GLuint p_framebuffer, GLuint* p_texture) {
				_generateMap(p_texture);
				glNamedFramebufferTexture(p_framebuffer, GL_DEPTH_ATTACHMENT, *p_texture, 0);
			}

			void _resizeColorMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_2D, p_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
			}

			void _resizeDepthMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_2D, p_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			}
			
			void _resizeColorCubeMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, p_texture);
				for (unsigned int i = 0; i < 6; i++)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, NULL);
			}

			void _resizeDepthCubeMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, p_texture);
				for (unsigned int i = 0; i < 6; i++)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			}

		private:
			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void _compileShader(GLuint p_shader, std::string p_path) {
				std::ifstream ifstream(p_path, std::ifstream::in);
				if (!ifstream.is_open()) throw std::ios_base::failure("Cannot open file: " + p_path);
				std::stringstream stream;
				stream << ifstream.rdbuf();
				ifstream.close();
				const std::string file = stream.str();

				const GLchar* shaderSrc = file.c_str();
				glShaderSource(p_shader, 1, &shaderSrc, NULL);
				glCompileShader(p_shader);
			}
		};
	}
}
#endif