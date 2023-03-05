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

				glCreateVertexArrays(1, &_vao);
				glCreateBuffers(1, &_vbo);
				glNamedBufferData(_vbo, 6*sizeof(Vec2f), nullptr, GL_DYNAMIC_DRAW);
				glEnableVertexArrayAttrib(_vao, 0);
				glVertexArrayAttribFormat(_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_vao, 0, 0);	
				glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(Vec2f));
			}
			~ShadingPassOGL() {
				glDisableVertexArrayAttrib(_vao, 0);
				glDeleteVertexArrays(1, &_vao);
				glDeleteBuffers(1, &_vbo);
			}

			void resize(int p_width, int p_height) override {}

			Vec2f projectBillboard(float x, float y, Mat4f MVP) {
				Vec4f tmp = Vec4f(x, y, 1., 1.)* MVP;
				return Vec2f(tmp.x, tmp.y);
			}

			void execute(int p_viewport_width, int p_viewport_height, GLuint p_positionMetalnessMap, GLuint p_normalRoughnessMap, GLuint p_albedoMap) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_viewport_width, p_viewport_height);

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glDisable(GL_DEPTH_TEST);

				glUseProgram(_program);

				glProgramUniform3fv(_program, _uCamPosLoc, 1, glm::value_ptr(Application::getInstance().getSceneManager().getCamera().getPosition()));

				/*for (Light* l : Application::getInstance().getSceneManager().getLights()) {
					glBindTextureUnit(0, p_positionMetalnessMap);
					glBindTextureUnit(1, p_normalRoughnessMap);
					glBindTextureUnit(2, p_albedoMap);

					Vec3f pos = l->getPosition();
					float r = l->getRange();
					Mat3f VP = Application::getInstance().getSceneManager().getCamera().getProjectionMatrix()* Application::getInstance().getSceneManager().getCamera().getViewMatrix();

					glProgramUniform4fv(_program, _uLightDirectionLoc, 1, glm::value_ptr(Vec4f(l->getDirection(), l->getInnerConeAngle())));
					glProgramUniform4fv(_program, _uLightEmissivityLoc, 1, glm::value_ptr(Vec4f(l->getEmissivity(), l->getOuterConeAngle())));

					switch(l->getType()) {
						case LIGHT_TYPE::POINT : 
						case LIGHT_TYPE::SPOT :
							glProgramUniform4fv(_program, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(pos, 1.)));*/
							/*glNamedBufferSubData(_vbo, 0, 6 * sizeof(Vec2f), new Vec2f[]{
								projectBillboard(pos.x - r,pos.y - r,VP) , projectBillboard(pos.x + r,pos.y - r,VP) , projectBillboard(pos.x + r,pos.y + r,VP),
								projectBillboard(pos.x + r,pos.y + r,VP) , projectBillboard(pos.x - r,pos.y + r,VP) , projectBillboard(pos.x - r,pos.y - r,VP) });*/
							/*glNamedBufferSubData(_vbo, 0, 6 * sizeof(Vec2f), new Vec2f[]{Vec2f(-1.,-1.),Vec2f(1.,-1.),Vec2f(1.,1.),Vec2f(1.,1.),Vec2f(-1.,1.),Vec2f(-1.,-1.)});
							break;
						default :
							glProgramUniform4fv(_program, _uLightPositionLoc, 1, glm::value_ptr(Vec4f(pos, 0.)));
							glNamedBufferSubData(_vbo, 0, 6*sizeof(Vec2f), new Vec2f[]{Vec2f(-1.,-1.),Vec2f(1.,-1.),Vec2f(1.,1.),Vec2f(1.,1.),Vec2f(-1.,1.),Vec2f(-1.,-1.) });
					}

					glBindVertexArray(_vao);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glBindVertexArray(0);
				}*/
			}

		private:
			GLuint _fbo					= GL_INVALID_INDEX;

			GLuint _vao					= GL_INVALID_INDEX;
			GLuint _vbo					= GL_INVALID_INDEX;
			GLuint _ebo					= GL_INVALID_INDEX;

			GLuint _uCamPosLoc			= GL_INVALID_INDEX;
			GLuint _uLightPositionLoc	= GL_INVALID_INDEX;
			GLuint _uLightDirectionLoc	= GL_INVALID_INDEX;
			GLuint _uLightEmissivityLoc = GL_INVALID_INDEX;
		};
	}
}
#endif