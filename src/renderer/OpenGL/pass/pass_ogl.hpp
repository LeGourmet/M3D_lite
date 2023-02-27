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
			~PassOGL() { glDeleteProgram(_program); }

			virtual void resize(int p_width, int p_height) = 0;

		protected:
			GLuint _program = GL_INVALID_INDEX;
			
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

			void _generateAndAttachMap(GLuint* p_texture, int p_id) {
				glGenTextures(1, p_texture);
				glBindTexture(GL_TEXTURE_2D, *p_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1, 1, 0, GL_RGBA, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + p_id, *p_texture, 0);
			}
	};
}
}
#endif
