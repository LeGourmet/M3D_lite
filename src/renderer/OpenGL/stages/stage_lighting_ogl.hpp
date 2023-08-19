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

#include <iostream>

namespace M3D
{
	namespace Renderer
	{
		class StageLightingOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageLightingOGL() {
				glCreateFramebuffers(1, &_fboLighting);
				generateMap(&_lightingMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fboLighting, _lightingMap, 0);
				
				// --- ambient lighting ---
				_ambientLightingPass.addUniform("uCamPos");
								
				// --- directional lighting ---
				_directionalLightingPass.addUniform("uCamPos");
				_directionalLightingPass.addUniform("uLightMatrix_VP");
				_directionalLightingPass.addUniform("uLightDirection");
				_directionalLightingPass.addUniform("uLightEmissivity");
				
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

				// --- ponctual lighting ---
				_ponctualLightingPass.addUniform("uCamData");
				_ponctualLightingPass.addUniform("uLightPosition");
				_ponctualLightingPass.addUniform("uLightDirection");
				_ponctualLightingPass.addUniform("uLightEmissivity");
				_ponctualLightingPass.addUniform("uLightCosAngles");

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

				// --- others ---
				glCreateVertexArrays(1, &_emptyVAO);
				glCreateBuffers(1, &_billboardSSBO);
				glNamedBufferStorage(_billboardSSBO, 4*sizeof(Vec4f), nullptr, GL_DYNAMIC_STORAGE_BIT);
			}

			~StageLightingOGL() { 
				glDeleteTextures(1, &_lightingMap);
				glDeleteTextures(1, &_shadowCubeMap);
				glDeleteTextures(1, &_shadowMap);

				glDeleteFramebuffers(1, &_fboLighting);
				glDeleteFramebuffers(1, &_fboShadowCube);
				glDeleteFramebuffers(1, &_fboShadow);

				glDeleteVertexArrays(1, &_emptyVAO);
				glDeleteBuffers(1, &_billboardSSBO);
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getLightingMap() { return _lightingMap; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _lightingMap);
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_positionMap, GLuint p_normalMetalnessMap, GLuint p_albedoRoughnessMap, GLuint p_fbo) {
				glNamedFramebufferReadBuffer(p_fbo, GL_COLOR_ATTACHMENT3);
				glNamedFramebufferDrawBuffer(_fboLighting, GL_COLOR_ATTACHMENT0);
				glBlitNamedFramebuffer(p_fbo,_fboLighting,0,0,p_width,p_height,0,0,p_width,p_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

				// --- ambient lighting ---
				glViewport(0, 0, p_width, p_height);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);

				glUseProgram(_ambientLightingPass.getProgram());

				glBindTextureUnit(0, p_positionMap);
				glBindTextureUnit(1, p_normalMetalnessMap);
				glBindTextureUnit(2, p_albedoRoughnessMap);

				glProgramUniform3fv(_ambientLightingPass.getProgram(), _ambientLightingPass.getUniform("uCamPos"), 1, glm::value_ptr(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition()));

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);

				glDisable(GL_BLEND);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				for (Scene::Light l : Application::getInstance().getSceneManager().getLights())
					for (unsigned int i=0; i<l.getNumberInstances() ;i++)
						switch (l.getType()) {
							case LIGHT_TYPE::POINT:
							case LIGHT_TYPE::SPOT:
								{
									float znear = 1e-3f;
									float zfar = glm::max(znear,l.getRange()); // TODO frustum culling and bounding sphere objects to compute zfar
									Vec3f lightPos = l.getInstance(i)->getPosition();

									// --- shadow cube ---
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
										for (unsigned int j=0; j < mesh.first->getSubMeshes().size(); j++) {
											Scene::SubMesh subMesh = mesh.first->getSubMeshes()[j];
											if (!subMesh.getMaterial().isOpaque()) continue;

											if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
											else { glDisable(GL_CULL_FACE); }

											glProgramUniform4fv(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
											glProgramUniform1f(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uAlphaCutOff"), subMesh.getMaterial().getAlphaCutOff());
											glProgramUniform1i(_shadowCubePass.getProgram(), _shadowCubePass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
											if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

											mesh.second->bind(j);
											glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)subMesh.getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
											glBindVertexArray(0);
										}

									glDisable(GL_CULL_FACE);
									glDisable(GL_DEPTH_TEST);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									// --- lighting ---
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
									glProgramUniform3fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightPosition"), 1, glm::value_ptr(l.getInstance(i)->getPosition()));
									glProgramUniform3fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightDirection"), 1, glm::value_ptr(l.getInstance(i)->getFront()));
									glProgramUniform3fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l.getEmissivity()));
									glProgramUniform2fv(_ponctualLightingPass.getProgram(), _ponctualLightingPass.getUniform("uLightCosAngles"), 1, glm::value_ptr(Vec2f(l.getCosInnerConeAngle(), l.getCosOuterConeAngle())));
									
									/*Mat4f p_matrixVP = Application::getInstance().getSceneManager().getMainCameraViewMatrix() * Application::getInstance().getSceneManager().getMainCameraProjectionMatrix();

									Vec4f minBillboard = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getUp() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getRight())*l.getRange() + lightPos,1.f);
									minBillboard /= minBillboard.a;
									
									Vec4f maxBillboard = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getDown() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getLeft())*l.getRange() + lightPos,1.f);
									maxBillboard /= maxBillboard.a;

									_billBoardCoords[0] = glm::clamp(Vec4f(minBillboard.x, minBillboard.y, minBillboard.z, 1.), -VEC4F_ONE, VEC4F_ONE);
									_billBoardCoords[1] = glm::clamp(Vec4f(maxBillboard.x, minBillboard.y, (minBillboard.z + maxBillboard.z) * 0.5, 1.), -VEC4F_ONE, VEC4F_ONE);
									_billBoardCoords[2] = glm::clamp(Vec4f(minBillboard.x, maxBillboard.y, (minBillboard.z + maxBillboard.z) * 0.5, 1.), -VEC4F_ONE, VEC4F_ONE);
									_billBoardCoords[3] = glm::clamp(Vec4f(maxBillboard.x, maxBillboard.y, maxBillboard.z, 1.), -VEC4F_ONE, VEC4F_ONE);*/
									
									_billBoardCoords[0] = Vec4f(-1.,-1.,0.,1.);
									_billBoardCoords[1] = Vec4f( 1.,-1.,0.,1.);
									_billBoardCoords[2] = Vec4f(-1., 1.,0.,1.);
									_billBoardCoords[3] = Vec4f( 1., 1.,0.,1.);
									
									glNamedBufferSubData(_billboardSSBO, 0, 4*sizeof(Vec4f), &_billBoardCoords);
									glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _billboardSSBO);

									glBindVertexArray(_emptyVAO);
									glDrawArrays(GL_TRIANGLES, 0, 6);
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
									float znear = 1e-3f;
									float xmag = 50.f, ymag = 50.f, zfar = 50.f;
									Vec3f centerFrustum = VEC3F_ZERO;

									Mat4f lightMatrix_VP = glm::ortho(-0.5f*xmag, 0.5f*xmag, -0.5f*ymag, 0.5f*ymag, znear, zfar) * glm::lookAt(centerFrustum+l.getInstance(i)->getBack()*0.5f*zfar, centerFrustum, l.getInstance(i)->getUp());
									glProgramUniformMatrix4fv(_shadowPass.getProgram(), _shadowPass.getUniform("uLightMatrix_VP"), 1, false, glm::value_ptr(lightMatrix_VP));
									
									for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
										for (unsigned int j=0; j<mesh.first->getSubMeshes().size(); j++) {
											Scene::SubMesh subMesh = mesh.first->getSubMeshes()[j];
											if (!subMesh.getMaterial().isOpaque()) continue;

											if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
											else { glDisable(GL_CULL_FACE); }

											glProgramUniform4fv(_shadowPass.getProgram(), _shadowPass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
											glProgramUniform1f(_shadowPass.getProgram(), _shadowPass.getUniform("uAlphaCutOff"), subMesh.getMaterial().getAlphaCutOff());
											glProgramUniform1i(_shadowPass.getProgram(), _shadowPass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
											if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

											mesh.second->bind(j);
											glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)subMesh.getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
											glBindVertexArray(0);
										}

									glDisable(GL_CULL_FACE);
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
									glProgramUniform3fv(_directionalLightingPass.getProgram(), _directionalLightingPass.getUniform("uLightDirection"), 1, glm::value_ptr(l.getInstance(i)->getFront()));
									glProgramUniform3fv(_directionalLightingPass.getProgram(), _directionalLightingPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l.getEmissivity()));

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

			Vec4f  _billBoardCoords[4] = {VEC4F_ZERO, VEC4F_ZERO, VEC4F_ZERO, VEC4F_ZERO};
			GLuint _billboardSSBO   = GL_INVALID_INDEX;
			GLuint _emptyVAO		= GL_INVALID_INDEX;

			unsigned int _sadowMapResolution = 1024;

			ProgramOGL _ambientLightingPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/ambientLightingPass.frag");
			ProgramOGL _directionalLightingPass = ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/directionalLightingPass.frag");
			ProgramOGL _ponctualLightingPass	= ProgramOGL("src/renderer/OpenGL/shaders/utils/billboard.vert", "", "src/renderer/OpenGL/shaders/lighting/ponctualLightingPass.frag");
			ProgramOGL _shadowPass				= ProgramOGL("src/renderer/OpenGL/shaders/shadow/shadow.vert", "", "src/renderer/OpenGL/shaders/shadow/shadow.frag");
			ProgramOGL _shadowCubePass			= ProgramOGL("src/renderer/OpenGL/shaders/shadow/cubeShadow.vert", "src/renderer/OpenGL/shaders/shadow/cubeShadow.geom", "src/renderer/OpenGL/shaders/shadow/cubeShadow.frag");

		};
	}
}

#endif
