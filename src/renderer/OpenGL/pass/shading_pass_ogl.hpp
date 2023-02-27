#ifndef __SHADING_PASS_OGL_HPP__
#define __SHADING_PASS_OGL_HPP__

#include "pass_ogl.hpp"

#include "glm/gtc/type_ptr.hpp"

namespace M3D
{
	namespace Renderer
	{
		class ShadingPassOGL : public PassOGL {
		public:
			ShadingPassOGL(std::string p_pathVert, std::string p_pathFrag, GLuint p_ambientMap) : PassOGL(p_pathVert, p_pathFrag) {
				_uCamPosLoc				= glGetUniformLocation(_program, "uCamPos");
				_uLightPositionLoc		= glGetUniformLocation(_program, "uLightPosition");
				_uLightDirectionLoc		= glGetUniformLocation(_program, "uLightDirection");
				_uLightEmissivityLoc	= glGetUniformLocation(_program, "uLightEmissivity");

				glGenFramebuffers(1, &_fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, p_ambientMap, 0);
				GLenum DrawBuffers1[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, DrawBuffers1);
			}
			~ShadingPassOGL() {
				/*todo*/
			}

			void resize(int p_width, int p_height) override {}

			// sphere d'influence
			void execute(int p_viewport_width, int p_viewport_height, GLuint p_positionMetalnessMap, GLuint p_normalRoughnessMap, GLuint p_albedoMap, GLuint p_emptyVao) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_viewport_width, p_viewport_height);

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glDisable(GL_DEPTH_TEST);

				glUseProgram(_program);

				glProgramUniform3fv(_program, _uCamPosLoc, 1, glm::value_ptr(Application::getInstance().getSceneManager().getCamera().getPosition()));

				// remplacer par imposteur sphere d'influence + une light à la fois
				for (int i = 0; i<2 ;i++) {
					if (i == 0) {
						glProgramUniform4fv(_program, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(0., 0., 0., 0.)));
						glProgramUniform4fv(_program, _uLightDirectionLoc, 1, glm::value_ptr(Vec4f(-0.5, -0.5, -0.5, -1.)));
						glProgramUniform4fv(_program, _uLightEmissivityLoc, 1, glm::value_ptr(Vec4f(1., 1., 1., -1.)));
					}
					else if (i == 1) {
						glProgramUniform4fv(_program, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(0., 20., 0., 1.)));
						glProgramUniform4fv(_program, _uLightDirectionLoc, 1, glm::value_ptr(Vec4f(0., -0.5, -0.5, -1.)));
						glProgramUniform4fv(_program, _uLightEmissivityLoc, 1, glm::value_ptr(Vec4f(0., 0., 500., -1.)));
					}
					else {
						glProgramUniform4fv(_program, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getCamera().getPosition(), 1.)));
						glProgramUniform4fv(_program, _uLightDirectionLoc, 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getCamera().getFront(), 0.9)));
						glProgramUniform4fv(_program, _uLightEmissivityLoc, 1, glm::value_ptr(Vec4f(500., 100., 100., 0.95)));
					}

					glBindTextureUnit(0, p_positionMetalnessMap);
					glBindTextureUnit(1, p_normalRoughnessMap);
					glBindTextureUnit(2, p_albedoMap);

					glBindVertexArray(p_emptyVao);
					glDrawArrays(GL_TRIANGLES, 0, 6);
				}
			}

		private:
			GLuint _fbo					= GL_INVALID_INDEX;

			GLuint _uCamPosLoc			= GL_INVALID_INDEX;
			GLuint _uLightPositionLoc	= GL_INVALID_INDEX;
			GLuint _uLightDirectionLoc	= GL_INVALID_INDEX;
			GLuint _uLightEmissivityLoc = GL_INVALID_INDEX;
		};
	}
}
#endif