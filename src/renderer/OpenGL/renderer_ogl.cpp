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
				_collectTexture();
				_computeShading();
				_applyPostProcess();

				SDL_GL_SwapWindow(p_window);
			}

			void RendererOGL::_collectTexture() {
				glBindFramebuffer(GL_FRAMEBUFFER, _fboBasePass);

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_basePass);

				const Mat4f VP = Application::getInstance().getSceneManager().getCamera().getProjectionMatrix() * Application::getInstance().getSceneManager().getCamera().getViewMatrix();

				for (Scene::MeshTriangle* mesh : Application::getInstance().getSceneManager().getMeshes()) {
					glProgramUniformMatrix4fv(_basePass, _uMatrix_MVPLoc, 1, false, glm::value_ptr(VP * mesh->_transformation));
					glProgramUniformMatrix4fv(_basePass, _uMatrix_MLoc, 1, false, glm::value_ptr(mesh->_transformation));
					glProgramUniformMatrix4fv(_basePass, _uMatrix_NormalLoc, 1, false, glm::value_ptr(glm::transpose(mesh->_transformation)));
					MeshOGL* meshOGL = _meshes.at(mesh);

					glProgramUniform1i(_basePass, _uHasAmbientMapLoc, mesh->_hasAmbientMap);
					glProgramUniform3fv(_basePass, _uAmbientLoc, 1, glm::value_ptr(mesh->_ka));
					if (mesh->_hasAmbientMap) glBindTextureUnit(0, meshOGL->getIdAmbientMap());

					glProgramUniform1i(_basePass, _uHasDiffuseMapLoc, mesh->_hasDiffuseMap);
					glProgramUniform3fv(_basePass, _uDiffuseLoc, 1, glm::value_ptr(mesh->_kd));
					if (mesh->_hasDiffuseMap) glBindTextureUnit(1, meshOGL->getIdDiffuseMap());

					glProgramUniform1i(_basePass, _uHasSpecularMapLoc, mesh->_hasSpecularMap);
					glProgramUniform3fv(_basePass, _uSpecularLoc, 1, glm::value_ptr(mesh->_ks));
					if (mesh->_hasSpecularMap)  glBindTextureUnit(2, meshOGL->getIdSpecularMap());

					glProgramUniform1i(_basePass, _uHasShininessMapLoc, mesh->_hasShininessMap);
					glProgramUniform1f(_basePass, _uShininessLoc, mesh->_s);
					if (mesh->_hasShininessMap) glBindTextureUnit(3, meshOGL->getIdShininessMap());

					glProgramUniform1i(_basePass, _uHasNormalMapLoc, mesh->_hasNormalMap);
					if (mesh->_hasNormalMap) glBindTextureUnit(4, meshOGL->getIdNormalMap());

					glBindVertexArray(meshOGL->getVao());
					glDrawElements(GL_TRIANGLES, (GLsizei)mesh->getIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}

			void RendererOGL::_computeShading() {
				glBindFramebuffer(GL_FRAMEBUFFER, _fboShadingPass);
				
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE,GL_ONE);
				glDisable(GL_DEPTH_TEST);

				glUseProgram(_shadingPass);
				
				glProgramUniform3fv(_shadingPass, _uCamPosLoc, 1, glm::value_ptr(Application::getInstance().getSceneManager().getCamera().getPosition()));
				for (int i=0; i<3 ;i++) {
					if (i==0) {
						glProgramUniform4fv(_shadingPass, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(0., 0., 0., 0.)));
						glProgramUniform4fv(_shadingPass, _uLightDirectionLoc, 1, glm::value_ptr(Vec4f(-0.5, -0.5, -0.5, -1.)));
						glProgramUniform4fv(_shadingPass, _uLightEmissivityLoc, 1, glm::value_ptr(Vec4f(1., 1., 1., -1.)));
					}
					else if(i==1){
						glProgramUniform4fv(_shadingPass, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(0., 20., 0., 1.)));
						glProgramUniform4fv(_shadingPass, _uLightDirectionLoc, 1, glm::value_ptr(Vec4f(0., -0.5, -0.5, -1.)));
						glProgramUniform4fv(_shadingPass, _uLightEmissivityLoc, 1, glm::value_ptr(Vec4f(0., 0., 200., -1.)));
					}
					else {
						glProgramUniform4fv(_shadingPass, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getCamera().getPosition(), 1.)));
						glProgramUniform4fv(_shadingPass, _uLightDirectionLoc, 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getCamera().getFront(), 0.9)));
						glProgramUniform4fv(_shadingPass, _uLightEmissivityLoc, 1, glm::value_ptr(Vec4f(500., 100., 100., 0.95)));
					}

					glBindTextureUnit(0, _albedoMap);
					glBindTextureUnit(1, _specularMap);
					glBindTextureUnit(2, _shininessMap);
					glBindTextureUnit(3, _normalMap);
					glBindTextureUnit(4, _positionMap);

					glBindVertexArray(_vaoEmpty);
					glDrawArrays(GL_TRIANGLES, 0, 6);
				}
				
			}

			void RendererOGL::_applyPostProcess() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glDisable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT);

				glUseProgram(_finalPass);
				
				glProgramUniform1f(_finalPass, _uGammaLoc, _gamma);
				glBindTextureUnit(0, _resultMap);
			
				glBindVertexArray(_vaoEmpty);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			void RendererOGL::_generateAndAttachMap(GLuint* p_texture, int p_id) {
				glGenTextures(1, p_texture);
				glBindTexture(GL_TEXTURE_2D, *p_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1280, 720, 0, GL_RGBA, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+p_id, *p_texture, 0);
			}

			std::string RendererOGL::_readShader(std::string p_path) {
				std::ifstream ifstream(p_path, std::ifstream::in);
				if (!ifstream.is_open()) throw std::ios_base::failure("Cannot open file: " + p_path);
				std::stringstream stream;
				stream << ifstream.rdbuf();
				ifstream.close();
				return stream.str();
			}

			void RendererOGL::_readCompileShader(GLuint p_shader, std::string p_path) {
				const std::string file = _readShader(p_path);
				const GLchar* shaderSrc = file.c_str();
				glShaderSource(p_shader, 1, &shaderSrc, NULL);
				glCompileShader(p_shader);
			}

			GLuint RendererOGL::_initProgram(std::string p_pathVert, std::string p_pathFrag) {
				GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
				GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

				_readCompileShader(vertexShader, p_pathVert);
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