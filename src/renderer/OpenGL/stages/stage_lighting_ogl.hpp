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
				glCreateFramebuffers(1, &_fboLighting);
				generateMap(&_lightingMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				attachColorMap(_fboLighting, _lightingMap, 0);

				// --- opaque directional ---
				_OpaqueDirectionalPass.addUniform("uCamData");
				_OpaqueDirectionalPass.addUniform("uLightMatrix_VP");
				_OpaqueDirectionalPass.addUniform("uLightDirection");
				_OpaqueDirectionalPass.addUniform("uLightEmissivity");
				
				// --- shadow directional ---
				_ShadowPass.addUniform("uLightMatrix_VP");
				_ShadowPass.addUniform("uAlbedo");
				_ShadowPass.addUniform("uAlphaCutOff");
				_ShadowPass.addUniform("uHasAlbedoMap");
				
				glCreateFramebuffers(1, &_fboShadow);
				generateMap(&_shadowMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTextureParameterfv(_shadowMap, GL_TEXTURE_BORDER_COLOR, borderColor);
				glTextureParameteri(_shadowMap, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTextureParameteri(_shadowMap, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				attachDepthMap(_fboShadow, _shadowMap);
				resizeDepthMap(_shadowMapResolution, _shadowMapResolution, _shadowMap);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboShadow);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// --- opaque punctual ---
				_OpaquePunctualPass.addUniform("uCamData");
				_OpaquePunctualPass.addUniform("uLightPosition");
				_OpaquePunctualPass.addUniform("uLightDirection");
				_OpaquePunctualPass.addUniform("uLightEmissivity");
				_OpaquePunctualPass.addUniform("uLightCosAngles");

				// --- shadow punctual ---
				_ShadowCubePass.addUniform("uShadowTransform");
				_ShadowCubePass.addUniform("uLightPos");
				_ShadowCubePass.addUniform("uZfar");
				_ShadowCubePass.addUniform("uAlbedo");
				_ShadowCubePass.addUniform("uAlphaCutOff");
				_ShadowCubePass.addUniform("uHasAlbedoMap");
				
				glCreateFramebuffers(1, &_fboShadowCube);
				generateCubeMap(&_shadowCubeMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				//glTextureParameteri(_shadowCubeMap, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				//glTextureParameteri(_shadowCubeMap, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				attachDepthMap(_fboShadowCube, _shadowCubeMap);
				resizeDepthCubeMap(_shadowMapResolution, _shadowMapResolution, _shadowCubeMap);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboShadowCube);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// --- transparent lighting ---
				_TransparentLightingPass.addUniform("uCamData");
				_TransparentLightingPass.addUniform("uLightPosition");
				_TransparentLightingPass.addUniform("uLightDirection");
				_TransparentLightingPass.addUniform("uLightEmissivity");
				_TransparentLightingPass.addUniform("uLightCosAngles");
				_TransparentLightingPass.addUniform("uLightTypePoint");
				
				// --- indirect lighting ---
				_IndirectLightingPass.addUniform("uCamPos");
				
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

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_positionMap, GLuint p_normalMetalnessMap, GLuint p_albedoRoughnessMap, GLuint p_emissiveMap, GLuint p_rootTransparency, GLuint p_ssboTransparency) {
				// --- clear buffer ---
				glCopyImageSubData(p_emissiveMap, GL_TEXTURE_2D, 0, 0, 0, 0, _lightingMap, GL_TEXTURE_2D, 0, 0, 0, 0, p_width, p_height, 1);
				
				/*
				- deterime point to cast ray shadow (point/spot => position ; sun depend of intersection of LDir and World AABB)
				- use compute shader to compute ray intersection with scene and determine for each fragments (opaque and transp) the quantity of light per channel that come
				- compute direct lighting opaque and transparent in unified shader
				*/

				// --- direct lighting ---
				for (Scene::Light l : Application::getInstance().getSceneManager().getLights())
					for (unsigned int i=0; i<l.getNumberInstances() ;i++)
						switch (l.getType()) {
							case LIGHT_TYPE::POINT:
							case LIGHT_TYPE::SPOT:
								{
									// todo better compute
									float znear = 1e-3f;
									float zfar = glm::max(znear,l.getRange()); // TODO frustum culling and bounding sphere objects to compute zfar
									Vec3f lightPos = l.getInstance(i)->getPosition();

									// --- shadow cube ---
									glViewport(0, 0, _shadowMapResolution, _shadowMapResolution);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboShadowCube);

									glEnable(GL_DEPTH_TEST);
									glClear(GL_DEPTH_BUFFER_BIT);

									glUseProgram(_ShadowCubePass.getProgram());

									Mat4f shadowProj = glm::perspective(PI_2f, 1.f, 1e-3f, zfar);
									std::vector<Mat4f> shadowTransforms = {
										shadowProj * glm::lookAt(lightPos, lightPos + VEC3F_X, -VEC3F_Y),
										shadowProj * glm::lookAt(lightPos, lightPos - VEC3F_X, -VEC3F_Y),
										shadowProj * glm::lookAt(lightPos, lightPos + VEC3F_Y,  VEC3F_Z),
										shadowProj * glm::lookAt(lightPos, lightPos - VEC3F_Y, -VEC3F_Z),
										shadowProj * glm::lookAt(lightPos, lightPos + VEC3F_Z, -VEC3F_Y),
										shadowProj * glm::lookAt(lightPos, lightPos - VEC3F_Z, -VEC3F_Y)
									};

									glProgramUniformMatrix4fv(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uShadowTransform"), 6, false, glm::value_ptr(shadowTransforms[0]));
									glProgramUniform3fv(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uLightPos"), 1, glm::value_ptr(lightPos));
									glProgramUniform1f(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uZfar"), zfar);

									for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
										// compute for each instance if they are cull or not => they id inside SSBO => render instance number = size SSBO => in shader read this ssbo with instance count
										for (unsigned int j=0; j < mesh.first->getSubMeshes().size(); j++) {
											Scene::SubMesh subMesh = mesh.first->getSubMeshes()[j];
											//if (!subMesh.getMaterial().isOpaque()) continue;

											if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
											else { glDisable(GL_CULL_FACE); }

											glProgramUniform4fv(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
											glProgramUniform1f(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uAlphaCutOff"), subMesh.getMaterial().getAlphaCutOff());
											glProgramUniform1i(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
											if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

											mesh.second->bind(j);
											glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)subMesh.getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
											glBindVertexArray(0);
										}

									glDisable(GL_CULL_FACE);
									glDisable(GL_DEPTH_TEST);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									Mat4f p_matrixVP = Application::getInstance().getSceneManager().getMainCameraProjectionMatrix() * Application::getInstance().getSceneManager().getMainCameraViewMatrix();

									Vec4f a = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getUp() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getLeft()) * l.getRange() + lightPos, 1.f);
									if (abs(a.a) > 0.01f) a /= a.a;

									Vec4f b = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getUp() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getRight()) * l.getRange() + lightPos, 1.f);
									if (abs(b.a) > 0.01f)  b /= b.a;

									Vec4f c = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getDown() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getLeft()) * l.getRange() + lightPos, 1.f);
									if (abs(c.a) > 0.01f)  c /= c.a;

									Vec4f d = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getDown() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getRight()) * l.getRange() + lightPos, 1.f);
									if (abs(d.a) > 0.01f)  d /= d.a;

									_billBoardCoords[0] = glm::clamp(Vec4f(a.x, a.y, 0., 1.), -VEC4F_ONE, VEC4F_ONE);
									_billBoardCoords[1] = glm::clamp(Vec4f(b.x, b.y, 0., 1.), -VEC4F_ONE, VEC4F_ONE);
									_billBoardCoords[2] = glm::clamp(Vec4f(c.x, c.y, 0., 1.), -VEC4F_ONE, VEC4F_ONE);
									_billBoardCoords[3] = glm::clamp(Vec4f(d.x, d.y, 0., 1.), -VEC4F_ONE, VEC4F_ONE);

									glNamedBufferSubData(_billboardSSBO, 0, 4 * sizeof(Vec4f), &_billBoardCoords);
									
									// --- opaque ---
									glViewport(0, 0, p_width, p_height);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

									glEnable(GL_BLEND);
									glBlendFunc(GL_ONE, GL_ONE);

									glUseProgram(_OpaquePunctualPass.getProgram());

									glBindTextureUnit(0, p_positionMap);
									glBindTextureUnit(1, p_normalMetalnessMap);
									glBindTextureUnit(2, p_albedoRoughnessMap);
									glBindTextureUnit(3, _shadowCubeMap);

									glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _billboardSSBO);

									glProgramUniform4fv(_OpaquePunctualPass.getProgram(), _OpaquePunctualPass.getUniform("uCamData"), 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition(), zfar)));
									glProgramUniform3fv(_OpaquePunctualPass.getProgram(), _OpaquePunctualPass.getUniform("uLightPosition"), 1, glm::value_ptr(l.getInstance(i)->getPosition()));
									glProgramUniform3fv(_OpaquePunctualPass.getProgram(), _OpaquePunctualPass.getUniform("uLightDirection"), 1, glm::value_ptr(l.getInstance(i)->getFront()));
									glProgramUniform3fv(_OpaquePunctualPass.getProgram(), _OpaquePunctualPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l.getEmissivity()));
									glProgramUniform2fv(_OpaquePunctualPass.getProgram(), _OpaquePunctualPass.getUniform("uLightCosAngles"), 1, glm::value_ptr(Vec2f(l.getCosInnerConeAngle(), l.getCosOuterConeAngle())));
									
									glBindVertexArray(_emptyVAO);
									glDrawArrays(GL_TRIANGLES, 0, 6);
									glBindVertexArray(0);

									glDisable(GL_BLEND);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									// --- transparent ---
									glViewport(0, 0, p_width, p_height);

									glUseProgram(_TransparentLightingPass.getProgram());

									glBindImageTexture(0, p_rootTransparency, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
									glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, p_ssboTransparency);

									glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _billboardSSBO);

									glProgramUniform4fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uCamData"), 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition(), zfar)));
									glProgramUniform3fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightPosition"), 1, glm::value_ptr(l.getInstance(i)->getPosition()));
									glProgramUniform3fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightDirection"), 1, glm::value_ptr(l.getInstance(i)->getFront()));
									glProgramUniform3fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l.getEmissivity()));
									glProgramUniform2fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightCosAngles"), 1, glm::value_ptr(Vec2f(l.getCosInnerConeAngle(), l.getCosOuterConeAngle())));
									glProgramUniform1i(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightTypePoint"), true);
									
									glBindVertexArray(_emptyVAO);
									glDrawArrays(GL_TRIANGLES, 0, 6);
									glBindVertexArray(0);

									break;
								}
							case LIGHT_TYPE::DIRECTIONAL:
								{
									// todo better compute
									float znear = 1e-3f;
									float xmag = 50.f, ymag = 50.f, zfar = 50.f;
									Vec3f centerFrustum = VEC3F_ZERO;
									Mat4f lightMatrix_VP = glm::ortho(-0.5f*xmag, 0.5f*xmag, -0.5f*ymag, 0.5f*ymag, znear, zfar) * glm::lookAt(centerFrustum+l.getInstance(i)->getBack()*0.5f*zfar, centerFrustum, l.getInstance(i)->getUp());
									Mat4f homogeneousLightMatrix_VP = Mat4f(0.5f, 0.f, 0.f, 0.f, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.5f, 0.f, 0.5f, 0.5f, 0.5f, 1.f) * lightMatrix_VP;
									
									// --- shadow ---
									glViewport(0, 0, _shadowMapResolution, _shadowMapResolution);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboShadow);

									glEnable(GL_DEPTH_TEST);
									glClear(GL_DEPTH_BUFFER_BIT);

									glUseProgram(_ShadowPass.getProgram());

									glProgramUniformMatrix4fv(_ShadowPass.getProgram(), _ShadowPass.getUniform("uLightMatrix_VP"), 1, false, glm::value_ptr(lightMatrix_VP));
									
									for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
										for (unsigned int j=0; j<mesh.first->getSubMeshes().size(); j++) {
											Scene::SubMesh subMesh = mesh.first->getSubMeshes()[j];
											//if (!subMesh.getMaterial().isOpaque()) continue;

											if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
											else { glDisable(GL_CULL_FACE); }

											glProgramUniform4fv(_ShadowPass.getProgram(), _ShadowPass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
											glProgramUniform1f(_ShadowPass.getProgram(), _ShadowPass.getUniform("uAlphaCutOff"), subMesh.getMaterial().getAlphaCutOff());
											glProgramUniform1i(_ShadowPass.getProgram(), _ShadowPass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
											if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

											mesh.second->bind(j);
											glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)subMesh.getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
											glBindVertexArray(0);
										}

									glDisable(GL_CULL_FACE);
									glDisable(GL_DEPTH_TEST);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									// --- opaque ---
									glViewport(0, 0, p_width, p_height);

									glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

									glEnable(GL_BLEND);
									glBlendFunc(GL_ONE, GL_ONE);

									glUseProgram(_OpaqueDirectionalPass.getProgram());

									glBindTextureUnit(0, p_positionMap);
									glBindTextureUnit(1, p_normalMetalnessMap);
									glBindTextureUnit(2, p_albedoRoughnessMap);
									glBindTextureUnit(3, _shadowMap);

									glProgramUniformMatrix4fv(_OpaqueDirectionalPass.getProgram(), _OpaqueDirectionalPass.getUniform("uLightMatrix_VP"), 1, false, glm::value_ptr(homogeneousLightMatrix_VP));
									glProgramUniform4fv(_OpaqueDirectionalPass.getProgram(), _OpaqueDirectionalPass.getUniform("uCamData"), 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition(), zfar)));
									glProgramUniform3fv(_OpaqueDirectionalPass.getProgram(), _OpaqueDirectionalPass.getUniform("uLightDirection"), 1, glm::value_ptr(l.getInstance(i)->getFront()));
									glProgramUniform3fv(_OpaqueDirectionalPass.getProgram(), _OpaqueDirectionalPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l.getEmissivity()));

									glBindVertexArray(_emptyVAO);
									glDrawArrays(GL_TRIANGLES, 0, 3);
									glBindVertexArray(0);

									glDisable(GL_BLEND);
									glBindFramebuffer(GL_FRAMEBUFFER, 0);

									// --- transparent ---
									_billBoardCoords[0] = Vec4f(-1., -1., 0., 1.);
									_billBoardCoords[1] = Vec4f( 1., -1., 0., 1.);
									_billBoardCoords[2] = Vec4f(-1.,  1., 0., 1.);
									_billBoardCoords[3] = Vec4f( 1.,  1., 0., 1.);

									glNamedBufferSubData(_billboardSSBO, 0, 4 * sizeof(Vec4f), &_billBoardCoords);

									glViewport(0, 0, p_width, p_height);

									glUseProgram(_TransparentLightingPass.getProgram());

									glBindImageTexture(0, p_rootTransparency, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
									glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, p_ssboTransparency);
									
									glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _billboardSSBO);
									
									glProgramUniform4fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uCamData"), 1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition(), zfar)));
									glProgramUniform3fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightDirection"), 1, glm::value_ptr(l.getInstance(i)->getFront()));
									glProgramUniform3fv(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l.getEmissivity()));
									glProgramUniform1i(_TransparentLightingPass.getProgram(), _TransparentLightingPass.getUniform("uLightTypePoint"), false);

									glBindVertexArray(_emptyVAO);
									glDrawArrays(GL_TRIANGLES, 0, 6);
									glBindVertexArray(0);

									break;
								}
						}

				// --- indirect lighting ---
				glViewport(0, 0, p_width, p_height);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

				glUseProgram(_IndirectLightingPass.getProgram());

				glBindTextureUnit(0, p_positionMap);
				glBindTextureUnit(1, p_normalMetalnessMap);
				glBindTextureUnit(2, p_albedoRoughnessMap);
				glBindTextureUnit(3, _lightingMap);

				glProgramUniform3fv(_IndirectLightingPass.getProgram(), _IndirectLightingPass.getUniform("uCamPos"), 1, glm::value_ptr(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition()));

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);

				// --- Final Mix ---
				glUseProgram(_FinalMixLightingPass.getProgram());

				glBindTextureUnit(0, _lightingMap);
				//+ indirect lighting map
				glBindImageTexture(1, p_rootTransparency, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, p_ssboTransparency);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fboLighting			= GL_INVALID_INDEX;
			GLuint _fboShadowCube		= GL_INVALID_INDEX;
			GLuint _fboShadow			= GL_INVALID_INDEX;

			GLuint _lightingMap			= GL_INVALID_INDEX;
			GLuint _shadowCubeMap		= GL_INVALID_INDEX;
			GLuint _shadowMap			= GL_INVALID_INDEX;

			Vec4f  _billBoardCoords[4]	= {VEC4F_ZERO, VEC4F_ZERO, VEC4F_ZERO, VEC4F_ZERO};
			GLuint _billboardSSBO		= GL_INVALID_INDEX;
			GLuint _emptyVAO			= GL_INVALID_INDEX;

			unsigned int _shadowMapResolution = 1024;

			ProgramOGL _ShadowPass					= ProgramOGL("src/renderer/OpenGL/shaders/shadow/Shadow.vert", "", "src/renderer/OpenGL/shaders/shadow/Shadow.frag");
			ProgramOGL _ShadowCubePass				= ProgramOGL("src/renderer/OpenGL/shaders/shadow/CubeShadow.vert", "src/renderer/OpenGL/shaders/shadow/CubeShadow.geom", "src/renderer/OpenGL/shaders/shadow/CubeShadow.frag");
			//ProgramOGL _GenerateShadowMaps		= ProgramOGL("src/renderer/OpenGL/shaders/shadow/QuadScreen.vert", "src/renderer/OpenGL/shaders/shadow/GenerateShadowMaps.frag"); // cast shadow ray (indirect call compute)  

			ProgramOGL _OpaqueDirectionalPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/OpaqueDirectionalPass.frag");
			ProgramOGL _OpaquePunctualPass			= ProgramOGL("src/renderer/OpenGL/shaders/utils/Billboard.vert", "", "src/renderer/OpenGL/shaders/lighting/OpaquePunctualPass.frag");
			ProgramOGL _TransparentLightingPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/Billboard.vert", "", "src/renderer/OpenGL/shaders/lighting/TransparentLightingPass.frag");
			//ProgramOGL _DirectLightingPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/Billboard.vert", "", "src/renderer/OpenGL/shaders/lighting/DirectLightingPass.frag");


			ProgramOGL _IndirectLightingPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/IndirectLightingPass.frag");

			ProgramOGL _FinalMixLightingPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/FinalMixLightingPass.frag");

		};
	}
}

#endif
