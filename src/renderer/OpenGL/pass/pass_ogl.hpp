#ifndef __PASS_OGL_HPP__
#define __PASS_OGL_HPP__

#include "GL/gl3w.h"

#include <string>
#include <fstream>
#include <sstream>

namespace M3D
{
	namespace Renderer
	{
		class PassOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			PassOGL(std::string p_pathVert, std::string p_pathFrag) {
				GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
				GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

				_readCompileShader(vertexShader, p_pathVert);
				_readCompileShader(fragmentShader, p_pathFrag);

				_program = glCreateProgram();

				glAttachShader(_program, vertexShader);
				glAttachShader(_program, fragmentShader);

				glLinkProgram(_program);

				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);
			}

			PassOGL(std::string p_pathVert, std::string p_pathGeom, std::string p_pathFrag) {
				GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
				GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
				GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

				_readCompileShader(vertexShader, p_pathVert);
				_readCompileShader(geometryShader, p_pathGeom);
				_readCompileShader(fragmentShader, p_pathFrag);

				_program = glCreateProgram();

				glAttachShader(_program, vertexShader);
				glAttachShader(_program, geometryShader);
				glAttachShader(_program, fragmentShader);

				glLinkProgram(_program);

				glDeleteShader(vertexShader);
				glDeleteShader(geometryShader);
				glDeleteShader(fragmentShader);
			}
			
			~PassOGL() { glDeleteProgram(_program); }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			virtual void resize(int p_width, int p_height) = 0;

		protected:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _program = GL_INVALID_INDEX;

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			std::string _readShader(std::string p_path) {
				std::ifstream ifstream(p_path, std::ifstream::in);
				if (!ifstream.is_open()) throw std::ios_base::failure("Cannot open file: " + p_path);
				std::stringstream stream;
				stream << ifstream.rdbuf();
				ifstream.close();
				return stream.str();
			}

			void _readCompileShader(GLuint p_shader, std::string p_path) {
				const std::string file = _readShader(p_path);
				const GLchar* shaderSrc = file.c_str();
				glShaderSource(p_shader, 1, &shaderSrc, NULL);
				glCompileShader(p_shader);
			}

			void _generateAndAttachMap(GLuint p_framebuffer, GLuint* p_texture, int p_id) {
				glCreateTextures(GL_TEXTURE_2D, 1, p_texture);
				glTextureParameteri(*p_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				//glTextureStorage2D(*p_texture, 0, GL_RGBA32F, 1, 1); // pas bon car imuable => need resize justjust after
				glNamedFramebufferTexture(p_framebuffer, GL_COLOR_ATTACHMENT0 + p_id, *p_texture, 0);
			}
		};
	}
}
#endif