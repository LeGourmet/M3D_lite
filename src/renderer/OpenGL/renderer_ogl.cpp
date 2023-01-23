#include "renderer_ogl.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"

#include <fstream>
#include <sstream>

namespace M3D
{
	namespace Renderer
	{
			void RendererOGL::drawFrame(SDL_Window* p_window){
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				const Mat4f VP = Application::getInstance().getSceneManager().getCamera().getProjectionMatrix() * Application::getInstance().getSceneManager().getCamera().getViewMatrix();
				const Vec3f posCam = Application::getInstance().getSceneManager().getCamera().getPosition();
				glProgramUniform3f(_program, _uCamPosLoc, posCam.x, posCam.y, posCam.z);

				for (Scene::MeshTriangle* mesh : Application::getInstance().getSceneManager().getMeshes()) {
					glProgramUniformMatrix4fv(_program, _uMVPMatrixLoc, 1, false, glm::value_ptr(VP * mesh->_transformation));
					Object_OGL meshDatas = _objects.at(mesh);

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasAmbientMap"), mesh->_hasAmbientMap);
					glProgramUniform3fv(_program, glGetUniformLocation(_program, "uAmbient"), 1, glm::value_ptr(mesh->_ka));
					if (mesh->_hasAmbientMap) glBindTextureUnit(0, meshDatas.getAmbientMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasDiffuseMap"), mesh->_hasDiffuseMap);
					glProgramUniform3fv(_program, glGetUniformLocation(_program, "uDiffuse"), 1, glm::value_ptr(mesh->_kd));
					if (mesh->_hasDiffuseMap) glBindTextureUnit(1, meshDatas.getDiffuseMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasSpecularMap"), mesh->_hasSpecularMap);
					glProgramUniform3fv(_program, glGetUniformLocation(_program, "uSpecular"), 1, glm::value_ptr(mesh->_ks));
					if (mesh->_hasSpecularMap)  glBindTextureUnit(2, meshDatas.getSpecularMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasShininessMap"), mesh->_hasShininessMap);
					glProgramUniform1f(_program, glGetUniformLocation(_program, "uShininess"), mesh->_s);
					if (mesh->_hasShininessMap) glBindTextureUnit(3, meshDatas.getShininessMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasNormalMap"), mesh->_hasNormalMap);
					if (mesh->_hasNormalMap) glBindTextureUnit(4, meshDatas.getNormalMap());

					glBindVertexArray(meshDatas.getVao());
					glDrawElements(GL_TRIANGLES, (GLsizei)mesh->getIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
				SDL_GL_SwapWindow(p_window);
			}

			const GLchar* RendererOGL::_readShader(std::string p_path) {
				std::ifstream ifstream(p_path, std::ifstream::in);
				if (!ifstream.is_open()) throw std::ios_base::failure("Cannot open file: " + p_path);
				std::stringstream stream;
				stream << ifstream.rdbuf();
				ifstream.close();
				return stream.str().c_str();
			}

			void RendererOGL::_readCompileShader(GLuint p_shader, std::string p_path) {
				const GLchar* shaderSrc = _readShader(p_path);
				glShaderSource(p_shader, 1, &shaderSrc, NULL);
				glCompileShader(p_shader);
			}

			GLuint RendererOGL::_initProgram(std::string p_pathVert, std::string p_pathFrag) {
				const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
				_readCompileShader(vertexShader, p_pathVert);

				const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
				_readCompileShader(fragmentShader, p_pathFrag);

				GLuint program = glCreateProgram();

				glAttachShader(program, vertexShader);
				glAttachShader(program, fragmentShader);

				glLinkProgram(program);

				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);

				return program;
			}
		
	}
}