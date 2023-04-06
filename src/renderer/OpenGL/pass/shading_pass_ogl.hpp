#ifndef __SHADING_PASS_OGL_HPP__
#define __SHADING_PASS_OGL_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "pass_ogl.hpp"
#include "shadow_pass.hpp"
#include "cube_shadow_pass.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "scene/objects/lights/light.hpp"
#include "scene/objects/cameras/camera.hpp"


namespace M3D
{
	namespace Renderer
	{
		class ShadingPassOGL : public PassOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ShadingPassOGL(std::string p_pathVert, std::string p_pathFrag) : PassOGL(p_pathVert, p_pathFrag) {
				_cubeShadowPass = new CubeShadowPassOGL("src/renderer/OpenGL/shaders/cubeShadowMapPass.vert", "src/renderer/OpenGL/shaders/cubeShadowMapPass.geom", "src/renderer/OpenGL/shaders/cubeShadowMapPass.frag");
				_shadowPass = new ShadowPassOGL("src/renderer/OpenGL/shaders/quadScreen.vert", "src/renderer/OpenGL/shaders/finalPass.frag"); // TODO pas les bon shader (juste pour pas que ça crash)
				
				_uZfarLoc					= glGetUniformLocation(_program, "uZfar");
				_uCamPosLoc					= glGetUniformLocation(_program, "uCamPos");
				_uLightPositionTypeLoc		= glGetUniformLocation(_program, "uLightPositionType");
				_uLightDirectionInnerLoc	= glGetUniformLocation(_program, "uLightDirectionInner");
				_uLightEmissivityOuterLoc	= glGetUniformLocation(_program, "uLightEmissivityOuter");

				glCreateFramebuffers(1, &_fboShading);
				_generateAndAttachMap(_fboShading, &_shadingMap, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fboShading, 1, DrawBuffers);

				glCreateVertexArrays(1, &_vaoBillboard);
				glCreateBuffers(1, &_vboBillboard);

				glVertexArrayVertexBuffer(_vaoBillboard, 0, _vboBillboard, 0, sizeof(Vec2f));
				glEnableVertexArrayAttrib(_vaoBillboard, 0);
				glVertexArrayAttribFormat(_vaoBillboard, 0, 2, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_vaoBillboard, 0, 0);
				glNamedBufferData(_vboBillboard, 6 * sizeof(Vec2f), nullptr, GL_DYNAMIC_DRAW);
			}
			~ShadingPassOGL() {
				glDeleteTextures(1, &_shadingMap);
				glDisableVertexArrayAttrib(_vaoBillboard, 0);
				glDeleteBuffers(1, &_vboBillboard);
				glDeleteVertexArrays(1, &_vaoBillboard);
				glDeleteFramebuffers(1, &_fboShading);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getShadingMap() { return _shadingMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override {
				glBindTexture(GL_TEXTURE_2D, _shadingMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, _fboShading);
				glViewport(0, 0, p_width, p_height);
			}

			void execute(GLuint p_positionMap, GLuint p_normalMetalnessMap, GLuint p_albedoRoughnessMap, const std::map<Scene::Mesh*, MeshOGL*>& p_meshes_ogl) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fboShading);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				Vec3f postionCamera = Application::getInstance().getSceneManager().getMainCameraTransformation() * Vec4f(VEC3F_ZERO, 1.);
				Mat4f VP = Application::getInstance().getSceneManager().getMainCameraProjectionMatrix() * Application::getInstance().getSceneManager().getMainCameraViewMatrix();
				float zfar = 100.;

				std::vector<Vec2f> billBoardCoord;
				billBoardCoord.reserve(6);
				float type;

				int i = -1;
				for (Scene::Light* l : Application::getInstance().getSceneManager().getLights()) {
					i++;
					if (i != 2) continue;
					for(unsigned int i=0; i<l->getNumberInstances(); i++){
						Mat4f transformation = l->getInstance(i)->computeTransformation();
						Vec3f positionLight = transformation * Vec4f(VEC3F_ZERO, 1.);
						Vec3f directionLight = transformation * Vec4f(-VEC3F_Z, 0.);

						switch (l->getType()) {
							case LIGHT_TYPE::POINT:
							case LIGHT_TYPE::SPOT: {
								_cubeShadowPass->execute(zfar,positionLight,p_meshes_ogl);

								// pos +/- l->getRange() * cam.left/front/up
								// ======== projectOnBillboard ========
								// Vec2f tmp = p_MVP * Vec4f(p_wpos, 0., 1.);
								// return glm::clamp(tmp, -1.f, 1.f);

								billBoardCoord = { Vec2f(-1.,-1.),Vec2f(1.,-1.),Vec2f(1.,1.),Vec2f(1.,1.),Vec2f(-1.,1.),Vec2f(-1.,-1.) };
								type = 1.;
								break;
							}
							case LIGHT_TYPE::DIRECTIONAL: {
								billBoardCoord = { Vec2f(-1.,-1.),Vec2f(1.,-1.),Vec2f(1.,1.),Vec2f(1.,1.),Vec2f(-1.,1.),Vec2f(-1.,-1.) };
								type = 0.;
								break;
							}
						}
						
						glBindFramebuffer(GL_FRAMEBUFFER, _fboShading);

						glEnable(GL_BLEND);
						glBlendFunc(GL_ONE, GL_ONE);

						glUseProgram(_program);

						glBindTextureUnit(0, p_positionMap);
						glBindTextureUnit(1, p_normalMetalnessMap);
						glBindTextureUnit(2, p_albedoRoughnessMap);
						glBindTextureUnit(3, _shadowPass->getShadowMap());
						glBindTextureUnit(4, _cubeShadowPass->getCubeShadowMap());

						glProgramUniform1f(_program, _uZfarLoc, zfar);
						glProgramUniform3fv(_program, _uCamPosLoc, 1, glm::value_ptr(postionCamera));
						glProgramUniform4fv(_program, _uLightPositionTypeLoc, 1, glm::value_ptr(Vec4f(positionLight, type)));
						glProgramUniform4fv(_program, _uLightEmissivityOuterLoc, 1, glm::value_ptr(Vec4f(l->getEmissivity(), l->getCosOuterConeAngle())));
						glProgramUniform4fv(_program, _uLightDirectionInnerLoc, 1, glm::value_ptr(Vec4f(glm::normalize(directionLight), l->getCosInnerConeAngle())));
						glNamedBufferSubData(_vboBillboard, 0, 6 * sizeof(Vec2f), billBoardCoord.data());

						glBindVertexArray(_vaoBillboard);
						glDrawArrays(GL_TRIANGLES, 0, 6);
						glBindVertexArray(0);

						glDisable(GL_BLEND);
					}
				}
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fboShading = GL_INVALID_INDEX;

			GLuint _vaoBillboard = GL_INVALID_INDEX;
			GLuint _vboBillboard = GL_INVALID_INDEX;
			GLuint _eboBillboard = GL_INVALID_INDEX;

			GLuint _shadingMap = GL_INVALID_INDEX;

			GLuint _uZfarLoc = GL_INVALID_INDEX;
			GLuint _uCamPosLoc = GL_INVALID_INDEX;
			GLuint _uLightPositionTypeLoc = GL_INVALID_INDEX;
			GLuint _uLightDirectionInnerLoc = GL_INVALID_INDEX;
			GLuint _uLightEmissivityOuterLoc = GL_INVALID_INDEX;

			ShadowPassOGL* _shadowPass = nullptr;
			CubeShadowPassOGL* _cubeShadowPass = nullptr;
		};
	}
}
#endif