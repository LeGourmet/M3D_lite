#ifndef __SHADING_PASS_OGL_HPP__
#define __SHADING_PASS_OGL_HPP__

#include "pass_ogl.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "scene/objects/lights/light.hpp"
#include "scene/objects/cameras/camera.hpp"

#include "glm/gtc/type_ptr.hpp"

namespace M3D
{
	namespace Renderer
	{
		class ShadingPassOGL : public PassOGL {
		public:
			ShadingPassOGL(std::string p_pathVert, std::string p_pathFrag) : PassOGL(p_pathVert, p_pathFrag) {
				_uCamPosLoc					= glGetUniformLocation(_program, "uCamPos");
				_uLightPositionTypeLoc		= glGetUniformLocation(_program, "uLightPositionType");
				_uLightDirectionInnerLoc	= glGetUniformLocation(_program, "uLightDirectionInner");
				_uLightEmissivityOuterLoc	= glGetUniformLocation(_program, "uLightEmissivityOuter");

				glCreateFramebuffers(1, &_fbo);
				_generateAndAttachMap(_fbo, &_shadingMap, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fbo, 1, DrawBuffers);

				glCreateVertexArrays(1, &_vao);
				glCreateBuffers(1, &_vbo);

				glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(Vec2f));
				glEnableVertexArrayAttrib(_vao, 0);
				glVertexArrayAttribFormat(_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_vao, 0, 0);
				glNamedBufferData(_vbo, 6 * sizeof(Vec2f), nullptr, GL_DYNAMIC_DRAW);
			}
			~ShadingPassOGL() {
				glDeleteTextures(1, &_shadingMap);
				glDisableVertexArrayAttrib(_vao, 0);
				glDeleteBuffers(1, &_vbo);
				glDeleteVertexArrays(1, &_vao);
				glDeleteFramebuffers(1, &_fbo);
			}

			GLuint getShadingMap() { return _shadingMap; }

			void resize(int p_width, int p_height) override {
				glBindTexture(GL_TEXTURE_2D, _shadingMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_width, p_height);
			}

			/*Vec2f projectBillboard(float x, float y, Mat4f MVP) {
				Vec4f tmp = Vec4f(x, y, 1., 1.)* MVP;
				return glm::clamp(Vec2f(tmp.x, tmp.y),-1.f,1.f);
			}*/

			void execute(GLuint p_positionMetalnessMap, GLuint p_normalRoughnessMap, GLuint p_albedoMap) {
				/*glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_program);

				glProgramUniform3fv(_program, _uCamPosLoc, 1, glm::value_ptr(Application::getInstance().getSceneManager().getCamera().getPosition()));

				for (Scene::Light* l : Application::getInstance().getSceneManager().getLights()) {
					glBindTextureUnit(0, p_positionMetalnessMap);
					glBindTextureUnit(1, p_normalRoughnessMap);
					glBindTextureUnit(2, p_albedoMap);

					Mat4f transformation = l->getSceneGraphNode()[0]->computeTransformation();
					Vec4f pos = transformation * Vec4f(0., 0., 0., 1.);
					Vec4f dir = transformation * Vec4f(0., 0., -1., 0.);

					//float r = l->getRange();
					//Mat3f VP = Application::getInstance().getSceneManager().getCamera().getProjectionMatrix()* Application::getInstance().getSceneManager().getCamera().getViewMatrix();

					glProgramUniform4fv(_program, _uLightDirectionInnerLoc, 1, glm::value_ptr(Vec4f(glm::normalize(Vec3f(dir.x, dir.y, dir.z)), l->getCosInnerConeAngle())));
					glProgramUniform4fv(_program, _uLightEmissivityOuterLoc, 1, glm::value_ptr(Vec4f(l->getEmissivity(), l->getCosOuterConeAngle())));

					Vec2f billBoardCoord[] = { Vec2f(-1.,-1.),Vec2f(1.,-1.),Vec2f(1.,1.),Vec2f(1.,1.),Vec2f(-1.,1.),Vec2f(-1.,-1.) };

					switch (l->getType()) {
					case LIGHT_TYPE::POINT:
					case LIGHT_TYPE::SPOT:
						glProgramUniform4fv(_program, _uLightPositionTypeLoc, 1, glm::value_ptr(Vec4f(pos.x, pos.y, pos.z, 1.)));
						glNamedBufferSubData(_vbo, 0, 6 * sizeof(Vec2f), &billBoardCoord);
						break;
					default:
						glProgramUniform4fv(_program, _uLightPositionTypeLoc, 1, glm::value_ptr(Vec4f(pos.x, pos.y, pos.z, 0.)));
						glNamedBufferSubData(_vbo, 0, 6 * sizeof(Vec2f), &billBoardCoord);
					}

					glBindVertexArray(_vao);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glBindVertexArray(0);
				}

				glDisable(GL_BLEND);*/
			}

		private:
			GLuint _fbo = GL_INVALID_INDEX;

			GLuint _vao = GL_INVALID_INDEX;
			GLuint _vbo = GL_INVALID_INDEX;
			GLuint _ebo = GL_INVALID_INDEX;

			GLuint _shadingMap = GL_INVALID_INDEX;

			GLuint _uCamPosLoc = GL_INVALID_INDEX;
			GLuint _uLightPositionTypeLoc = GL_INVALID_INDEX;
			GLuint _uLightDirectionInnerLoc = GL_INVALID_INDEX;
			GLuint _uLightEmissivityOuterLoc = GL_INVALID_INDEX;
		};
	}
}
#endif