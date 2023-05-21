#ifndef __STAGE_LIGHTING_OGL_HPP__
#define __STAGE_LIGHTING_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"

#include "stage_ogl.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "scene/objects/lights/light.hpp"
#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class StageLightingOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageLightingOGL() {
				// ****** LIGHTING ******
				_directionalLightingPass.addUniform("uCamPos");
				_directionalLightingPass.addUniform("uLightMatrix_VP");
				_directionalLightingPass.addUniform("uLightDirection");
				_directionalLightingPass.addUniform("uLightEmissivity");

				_ponctualLightingPass.addUniform("uCamData");
				_ponctualLightingPass.addUniform("uLightPosition");
				_ponctualLightingPass.addUniform("uLightDirection");
				_ponctualLightingPass.addUniform("uLightEmissivity");
				_ponctualLightingPass.addUniform("uLightCosAngles");

				glCreateFramebuffers(1, &_fboLighting);
				generateMap(&_lightingMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fboLighting, _lightingMap, 0);
				
				// ****** SHADOW ******
				_shadowPass.addUniform("uLightMatrix_VP");
				_shadowPass.addUniform("uAlbedo");
				_shadowPass.addUniform("uAlphaCutOff");
				_shadowPass.addUniform("uHasAlbedoMap");
				
				glCreateFramebuffers(1, &_fboShadow);
				generateMap(&_shadowMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTextureParameterfv(_shadowMap, GL_TEXTURE_BORDER_COLOR, borderColor);
				glTextureParameteri(_shadowMap, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTextureParameteri(_shadowMap, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				attachDepthMap(_fboShadow, _shadowMap);
				resizeDepthMap(_sadowMapResolution, _sadowMapResolution, _shadowMap);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboShadow);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				
				// ****** CUBE SHADOW ******
				_shadowCubePass.addUniform("uShadowTransform");
				_shadowCubePass.addUniform("uLightPos");
				_shadowCubePass.addUniform("uZfar");
				_shadowCubePass.addUniform("uAlbedo");
				_shadowCubePass.addUniform("uAlphaCutOff");
				_shadowCubePass.addUniform("uHasAlbedoMap");
				
				glCreateFramebuffers(1, &_fboShadowCube);
				generateCubeMap(&_shadowCubeMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				//glTextureParameteri(_shadowCubeMap, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				//glTextureParameteri(_shadowCubeMap, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				attachDepthMap(_fboShadowCube, _shadowCubeMap);
				resizeDepthCubeMap(_sadowMapResolution, _sadowMapResolution, _shadowCubeMap);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboShadowCube);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// ****** OTHERS ******
				glCreateVertexArrays(1, &_emptyVAO);

				glCreateBuffers(1, &_billboardVBO);
				glCreateBuffers(1, &_billboardEBO);
				glCreateVertexArrays(1, &_billboardVAO);

				glVertexArrayVertexBuffer(_billboardVAO, 0, _billboardVBO, 0, sizeof(Vec3f));
				glEnableVertexArrayAttrib(_billboardVAO, 0);
				glVertexArrayAttribFormat(_billboardVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_billboardVAO, 0, 0);

				unsigned int indices[] = { 0,1,2,2,3,0 };
				glNamedBufferData(_billboardEBO, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
				glNamedBufferData(_billboardVBO, 4 * sizeof(Vec3f), nullptr, GL_DYNAMIC_DRAW);

				glVertexArrayElementBuffer(_billboardVAO, _billboardEBO);
			}

			~StageLightingOGL() { 
				glDeleteTextures(1, &_lightingMap);
				glDeleteTextures(1, &_shadowCubeMap);
				glDeleteTextures(1, &_shadowMap);

				glDisableVertexArrayAttrib(_billboardVAO, 0);
				glDeleteBuffers(1, &_billboardVBO);
				glDeleteBuffers(1, &_billboardEBO);
				glDeleteVertexArrays(1, &_billboardVAO);
				glDeleteVertexArrays(1, &_emptyVAO);

				glDeleteFramebuffers(1, &_fboLighting);
				glDeleteFramebuffers(1, &_fboShadowCube);
				glDeleteFramebuffers(1, &_fboShadow);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getLightingMap() { return _lightingMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _lightingMap);
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_positionMap, GLuint p_normalMetalnessMap, GLuint p_albedoRoughnessMap) {
				float znear = 1e-3f;

				glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				for (Scene::Light* l : Application::getInstance().getSceneManager().getLights())
					for (unsigned int i=0; i<l->getNumberInstances() ;i++)
						switch (l->getType()) {
							case LIGHT_TYPE::POINT:
							case LIGHT_TYPE::SPOT:
								{
									float zfar = glm::max(znear,l->getRange()); // TODO frustum culling and bounding sphere objects to compute zfar
									Vec3f lightPos = l->getInstance(i)->getPosition();

									// ****** SHADOW CUBE ******
									glViewport(0, 0, _sadowMapResolution, _sadowMapResolution);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboShadowCube);

									glEnable(GL_DEPTH_TEST);
									glClear(GL_DEPTH_BUFFER_BIT);

									glUseProgram(_shadowCubePass.getProgram());

									Mat4f shadowProj = glm::perspective(PI_2f, 1.f, 1e-3f, zfar);
									std::vector<Mat4f> shadowTransforms = {
										shadowProj * glm::lookAt(lightPos, lightPos + VEC3F_X, -VEC3F_Y),
										shadowProj * glm::lookAt(lightPos, lightPos - VEC3F_X, -VEC3F_Y),
										shadowProj * glm::lookAt(lightPos, lightPos + VEC3F_Y,  VEC3F_Z),
										shadowProj * glm::lookAt(lightPos, lightPos - VEC3F_Y, -VEC3F_Z),
										shadowProj * glm::lookAt(lightPos, lightPos + VEC3F_Z, -VEC3F_Y),
										shadowProj * glm::lookAt(lightPos, lightPos - VEC3F_Z, -VEC3F_Y)
									};

									glProgramUniformMatrix4fv(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uShadowTransform"), 6, false, glm::value_ptr(shadowTransforms[0]));
									glProgramUniform3fv(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uLightPos"), 1, glm::value_ptr(lightPos));
									glProgramUniform1f(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uZfar"), zfar);

									for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
										for (unsigned int j=0; j < mesh.first->getPrimitives().size(); j++) {
											Scene::Primitive* primitive = mesh.first->getPrimitives()[j];
											if (!primitive->getMaterial().isOpaque()) continue;

											glProgramUniform4fv(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uAlbedo"), 1, glm::value_ptr(primitive->getMaterial().getBaseColor()));
											glProgramUniform1f(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uAlphaCutOff"), primitive->getMaterial().getAlphaCutOff());
											glProgramUniform1i(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uHasAlbedoMap"), primitive->getMaterial().getBaseColorMap() != nullptr);
											if (primitive->getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(primitive->getMaterial().getBaseColorMap())->getId());

											mesh.second->bind(j);
											glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)primitive->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
											glBindVertexArray(0);
										}

									glDisable(GL_DEPTH_TEST);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									// ****** LIGHTING ******
									glViewport(0, 0, p_width, p_height);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

									glEnable(GL_BLEND);
									glBlendFunc(GL_ONE, GL_ONE);

									glUseProgram(_ponctualLightingPass.getProgram());

									glBindTextureUnit(0, p_positionMap);
									glBindTextureUnit(1, p_normalMetalnessMap);
									glBindTextureUnit(2, p_albedoRoughnessMap);
									glBindTextureUnit(3, _shadowCubeMap);

									glProgramUniform4fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uCamData"), 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition(), zfar)));
									glProgramUniform3fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightPosition"), 1, glm::value_ptr(l->getInstance(i)->getPosition()));
									glProgramUniform3fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightDirection"), 1, glm::value_ptr(l->getInstance(i)->getFront()));
									glProgramUniform3fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l->getEmissivity()));
									glProgramUniform2fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightCosAngles"), 1, glm::value_ptr(Vec2f(l->getCosInnerConeAngle(), l->getCosOuterConeAngle())));
									
									/*Mat4f p_matrixVP = Application::getInstance().getSceneManager().getMainCameraViewMatrix() * Application::getInstance().getSceneManager().getMainCameraProjectionMatrix();
									Vec4f minBillboard = p_matrixVP * Vec4f(p_instanceLight->getLeft() * p_light->getRange() + p_instanceLight->getDown() * p_light->getRange(), 1.);
									minBillboard /= minBillboard.a;
									Vec4f maxBillboard = p_matrixVP * Vec4f(p_instanceLight->getRight() * p_light->getRange() + p_instanceLight->getUp() * p_light->getRange(), 1.);
									maxBillboard /= maxBillboard.a;

									_billBoardCoord[0] = glm::clamp(Vec3f(minBillboard.x, minBillboard.y, minBillboard.z), -VEC3F_ONE, VEC3F_ONE);
									_billBoardCoord[1] = glm::clamp(Vec3f(maxBillboard.x, minBillboard.y, (minBillboard.z + maxBillboard.z) * 0.5), -VEC3F_ONE, VEC3F_ONE);
									_billBoardCoord[2] = glm::clamp(Vec3f(maxBillboard.x, maxBillboard.y, maxBillboard.z), -VEC3F_ONE, VEC3F_ONE);
									_billBoardCoord[3] = glm::clamp(Vec3f(minBillboard.x, maxBillboard.y, (minBillboard.z + maxBillboard.z) * 0.5), -VEC3F_ONE, VEC3F_ONE);*/

									/*glNamedBufferSubData(_vbo, 0, 4 * sizeof(Vec3f), _billBoardCoord);

									glBindVertexArray(_vao);
									glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
									glBindVertexArray(0);*/

									glBindVertexArray(_emptyVAO);
									glDrawArrays(GL_TRIANGLES, 0, 3);
									glBindVertexArray(0);

									glDisable(GL_BLEND);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									break;
								}
							case LIGHT_TYPE::DIRECTIONAL:
								{
									// ****** SHADOW ******
									glViewport(0, 0, _sadowMapResolution, _sadowMapResolution);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboShadow);

									glEnable(GL_DEPTH_TEST);
									glClear(GL_DEPTH_BUFFER_BIT);

									glUseProgram(_shadowPass.getProgram());

									// todo better compute
									float xmag = 50.f, ymag = 50.f, zfar = 50.f;
									Vec3f centerFrustum = VEC3F_ZERO;

									Mat4f lightMatrix_VP = glm::ortho(-0.5f*xmag, 0.5f*xmag, -0.5f*ymag, 0.5f*ymag, znear, zfar) * glm::lookAt(centerFrustum+l->getInstance(i)->getBack()*0.5f*zfar, centerFrustum, l->getInstance(i)->getUp());
									glProgramUniformMatrix4fv(_shadowPass.getProgram(), _shadowPass.getUniform("uLightMatrix_VP"), 1, false, glm::value_ptr(lightMatrix_VP));
									
									for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
										for (unsigned int j=0; j<mesh.first->getPrimitives().size(); j++) {
											Scene::Primitive* primitive = mesh.first->getPrimitives()[j];
											if (!primitive->getMaterial().isOpaque()) continue;

											glProgramUniform4fv(_shadowPass.getProgram(), _shadowPass.getUniform("uAlbedo"), 1, glm::value_ptr(primitive->getMaterial().getBaseColor()));
											glProgramUniform1f(_shadowPass.getProgram(), _shadowPass.getUniform("uAlphaCutOff"), primitive->getMaterial().getAlphaCutOff());
											glProgramUniform1i(_shadowPass.getProgram(), _shadowPass.getUniform("uHasAlbedoMap"), primitive->getMaterial().getBaseColorMap() != nullptr);
											if (primitive->getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(primitive->getMaterial().getBaseColorMap())->getId());

											mesh.second->bind(j);
											glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)primitive->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
											glBindVertexArray(0);
										}

									glDisable(GL_DEPTH_TEST);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									// ****** LIGHTING ******
									glViewport(0, 0, p_width, p_height);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

									glEnable(GL_BLEND);
									glBlendFunc(GL_ONE, GL_ONE);

									glUseProgram(_directionalLightingPass.getProgram());

									glBindTextureUnit(0, p_positionMap);
									glBindTextureUnit(1, p_normalMetalnessMap);
									glBindTextureUnit(2, p_albedoRoughnessMap);
									glBindTextureUnit(3, _shadowMap);

									lightMatrix_VP = Mat4f(0.5,0.,0.,0.,0.,0.5,0.,0.,0.,0.,0.5,0.,0.5,0.5,0.5,1.) * lightMatrix_VP;
									glProgramUniformMatrix4fv(_directionalLightingPass.getProgram(), _directionalLightingPass.getUniform("uLightMatrix_VP"), 1, false, glm::value_ptr(lightMatrix_VP));
									glProgramUniform3fv(_directionalLightingPass.getProgram(), _directionalLightingPass.getUniform("uCamPos"), 1, glm::value_ptr(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition()));
									glProgramUniform3fv(_directionalLightingPass.getProgram(), _directionalLightingPass.getUniform("uLightDirection"), 1, glm::value_ptr(l->getInstance(i)->getFront()));
									glProgramUniform3fv(_directionalLightingPass.getProgram(), _directionalLightingPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l->getEmissivity()));

									glBindVertexArray(_emptyVAO);
									glDrawArrays(GL_TRIANGLES, 0, 3);
									glBindVertexArray(0);

									glDisable(GL_BLEND);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									break;
								}
						}
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fboLighting		= GL_INVALID_INDEX;
			GLuint _fboShadowCube	= GL_INVALID_INDEX;
			GLuint _fboShadow		= GL_INVALID_INDEX;

			GLuint _lightingMap		= GL_INVALID_INDEX;
			GLuint _shadowCubeMap	= GL_INVALID_INDEX;
			GLuint _shadowMap		= GL_INVALID_INDEX;

			Vec3f  _billboardCoords[4];
			GLuint _billboardVBO	= GL_INVALID_INDEX;
			GLuint _billboardEBO	= GL_INVALID_INDEX;
			GLuint _billboardVAO	= GL_INVALID_INDEX;
			GLuint _emptyVAO		= GL_INVALID_INDEX;

			unsigned int _sadowMapResolution = 1024;

			ProgramOGL _directionalLightingPass = ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/directionalLightingPass.frag");
			//ProgramOGL _ponctualLightingPass	= ProgramOGL("src/renderer/OpenGL/shaders/utils/billboard.vert", "", "src/renderer/OpenGL/shaders/lighting/ponctualLightingPass.frag");
			ProgramOGL _ponctualLightingPass	= ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/ponctualLightingPass.frag");
			ProgramOGL _shadowPass				= ProgramOGL("src/renderer/OpenGL/shaders/shadow/shadow.vert", "", "src/renderer/OpenGL/shaders/shadow/shadow.frag");
			ProgramOGL _shadowCubePass			= ProgramOGL("src/renderer/OpenGL/shaders/shadow/cubeShadow.vert", "src/renderer/OpenGL/shaders/shadow/cubeShadow.geom", "src/renderer/OpenGL/shaders/shadow/cubeShadow.frag");		
		};
	}
}
#endif