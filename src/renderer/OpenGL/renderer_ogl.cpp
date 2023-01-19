#include "renderer_ogl.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "scene/mesh_triangle.hpp"

#include <fstream>
#include <sstream>

namespace M3D
{
	namespace Renderer
	{
			void RendererOGL::drawFrame(SDL_Window* p_window){
				glUseProgram(_program);
				glEnable(GL_DEPTH_TEST);
				glClearColor(0., 0., 0., 1.); // define.hpp ? => here because color can change
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				const Mat4f MV = Application::getInstance().getSceneManager().getCamera().getProjectionMatrix() * Application::getInstance().getSceneManager().getCamera().getViewMatrix();
				const Vec3f posCam = Application::getInstance().getSceneManager().getCamera().getPosition();
				glProgramUniform3f(_program, _uCamPosLoc, posCam.x, posCam.y, posCam.z);

				for (Scene::MeshTriangle* mesh : Application::getInstance().getSceneManager().getMeshes()) {
					glProgramUniformMatrix4fv(_program, _uMVPMatrixLoc, 1, false, glm::value_ptr(MV * mesh->_transformation));

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasAmbientMap"), mesh->_hasAmbientMap);
					glProgramUniform3fv(_program, glGetUniformLocation(_program, "uAmbient"), 1, glm::value_ptr(mesh->_ka));
					if (mesh->_hasAmbientMap) glBindTextureUnit(0, (GLuint)mesh->getIdAmbientMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasDiffuseMap"), mesh->_hasDiffuseMap);
					glProgramUniform3fv(_program, glGetUniformLocation(_program, "uDiffuse"), 1, glm::value_ptr(mesh->_kd));
					if (mesh->_hasDiffuseMap) glBindTextureUnit(1, (GLuint)mesh->getIdDiffuseMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasSpecularMap"), mesh->_hasSpecularMap);
					glProgramUniform3fv(_program, glGetUniformLocation(_program, "uSpecular"), 1, glm::value_ptr(mesh->_ks));
					if (mesh->_hasSpecularMap)  glBindTextureUnit(2, (GLuint)mesh->getIdSpecularMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasShininessMap"), mesh->_hasShininessMap);
					glProgramUniform1f(_program, glGetUniformLocation(_program, "uShininess"), mesh->_s);
					if (mesh->_hasShininessMap) glBindTextureUnit(3, (GLuint)mesh->getIdShininessMap());

					glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasNormalMap"), mesh->_hasNormalMap);
					if (mesh->_hasNormalMap) glBindTextureUnit(4, (GLuint)mesh->getIdNormalMap());

					glBindVertexArray((GLuint)mesh->getIdVAO());
					glDrawElements(GL_TRIANGLES, (GLsizei)mesh->getIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
				SDL_GL_SwapWindow(p_window);
			}

			unsigned int RendererOGL::createTexture(const std::string p_path) {
				Texture_OGL* texture = new Texture_OGL(p_path); 
				_textures.push_back(texture);
				return texture->getId();
			}

			unsigned int RendererOGL::createVAO(const std::vector<Vertex> p_vertices, const std::vector<unsigned int> p_indices) {
				VAO_OGL* vao = new VAO_OGL(p_vertices, p_indices); 
				_VAOs.push_back(vao);
				return vao->getId();
			}
			
			void RendererOGL::deleteTexture(unsigned int p_id) {
				for (int i=0; i<_textures.size() ;i++)
					if (_textures[i]->getId() == p_id) {
						delete _textures[i];
						_textures.erase(_textures.begin() + i);
						break;
					}
			}
			
			void RendererOGL::deleteVAO(unsigned int p_id) {
				for (int i = 0; i < _VAOs.size();i++)
					if (_VAOs[i]->getId() == p_id) {
						delete _VAOs[i];
						_VAOs.erase(_VAOs.begin() + i);
						break;
					}
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