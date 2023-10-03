#ifndef __PROGRAM_OGL_HPP__
#define __PROGRAM_OGL_HPP__

#include "GL/gl3w.h"

#include <map>

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace M3D
{
	namespace Renderer
	{
		class ProgramOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ProgramOGL() = default;

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
			
			~ProgramOGL() { 
				glDeleteProgram(_program); 
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			inline GLuint getProgram() { return _program; }
			inline GLuint getUniform(std::string p_uniform) { return _uniformsLoc.at(p_uniform); }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void addUniform(std::string p_uniform) {
				_uniformsLoc.insert(std::pair<std::string, GLuint>(p_uniform, glGetUniformLocation(_program, p_uniform.c_str())));
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _program = GL_INVALID_INDEX;

			std::map<std::string, GLuint> _uniformsLoc;

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
