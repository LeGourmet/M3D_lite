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

				// --- direct lighting ---
				_DirectLightingPass.addUniform("uCamPos");
				_DirectLightingPass.addUniform("uInvMatrix_VP");
				_DirectLightingPass.addUniform("uShadowFar");
				_DirectLightingPass.addUniform("uLightMatrix_VP");
				_DirectLightingPass.addUniform("uLightPosition");
				_DirectLightingPass.addUniform("uLightDirection");
				_DirectLightingPass.addUniform("uLightEmissivity");
				_DirectLightingPass.addUniform("uLightCosAngles");
				_DirectLightingPass.addUniform("uLightTypePoint");

				// --- indirect lighting ---
				_IndirectLightingPass.addUniform("uCamPos");
				
				// --- others ---
				glCreateBuffers(1, &_billBoardVBO);
				glCreateVertexArrays(1, &_billBoardVAO);
				glNamedBufferData(_billBoardVBO, 4 * sizeof(Vec3f), nullptr, GL_DYNAMIC_DRAW);
				glVertexArrayVertexBuffer(_billBoardVAO, 0, _billBoardVBO, 0, sizeof(Vec3f));
				glEnableVertexArrayAttrib(_billBoardVAO, 0);
				glVertexArrayAttribFormat(_billBoardVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_billBoardVAO, 0, 0);

				glCreateVertexArrays(1, &_emptyVAO);
			}

			~StageLightingOGL() {
				glDeleteTextures(1, &_lightingMap);
				glDeleteTextures(1, &_shadowCubeMap);
				glDeleteTextures(1, &_shadowMap);

				glDeleteFramebuffers(1, &_fboLighting);
				glDeleteFramebuffers(1, &_fboShadowCube);
				glDeleteFramebuffers(1, &_fboShadow);

				glDeleteBuffers(1, &_billBoardVBO);
				glDeleteVertexArrays(1, &_billBoardVAO);
				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getLightingMap() { return _lightingMap; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(GL_RGB16F, GL_RGB, GL_FLOAT, p_width, p_height, _lightingMap);
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_albedoMap, GLuint p_normalMap, GLuint p_metalnessRoughnessMap, GLuint p_emissiveMap, GLuint p_depthMap, GLuint p_rootTransparency, GLuint p_ssboTransparency) {
				// --- clear buffer ---
				glCopyImageSubData(p_emissiveMap, GL_TEXTURE_2D, 0, 0, 0, 0, _lightingMap, GL_TEXTURE_2D, 0, 0, 0, 0, p_width, p_height, 1);
				
				// --- direct lighting ---
				for (Scene::Light l : Application::getInstance().getSceneManager().getLights())
					for (unsigned int i = 0; i < l.getNumberInstances();i++) {
						//-deterime point to cast ray shadow (point/spot => position ; sun depend of intersection of LDir and World AABB)
						//-use compute shader to compute ray intersection with scene and determine for each fragments(opaque and transp) the quantity of light per channel that come

						Mat4f lightMatrix_VP = glm::ortho(-25.f, 25.f, -25.f, 25.f, 1e-3f, 50.f) * glm::lookAt(VEC3F_ZERO + l.getInstance(i)->getBack() * 0.5f * 50.f, VEC3F_ZERO, l.getInstance(i)->getUp());
						Mat4f homogeneousLightMatrix_VP = Mat4f(0.5f, 0.f, 0.f, 0.f, 0.f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.5f, 0.f, 0.5f, 0.5f, 0.5f, 1.f) * lightMatrix_VP;

						if (l.getType() == LIGHT_TYPE::DIRECTIONAL) {
							glViewport(0, 0, _shadowMapResolution, _shadowMapResolution);
							glBindFramebuffer(GL_FRAMEBUFFER, _fboShadow);

							glEnable(GL_DEPTH_TEST);
							glClear(GL_DEPTH_BUFFER_BIT);

							glUseProgram(_ShadowPass.getProgram());

							glProgramUniformMatrix4fv(_ShadowPass.getProgram(), _ShadowPass.getUniform("uLightMatrix_VP"), 1, false, glm::value_ptr(lightMatrix_VP));

							for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
								for (unsigned int j = 0; j < mesh.first->getSubMeshes().size(); j++) {
									Scene::SubMesh subMesh = mesh.first->getSubMeshes()[j];
									
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
						}else{
							glViewport(0, 0, _shadowMapResolution, _shadowMapResolution);
							glBindFramebuffer(GL_FRAMEBUFFER, _fboShadowCube);

							glEnable(GL_DEPTH_TEST);
							glClear(GL_DEPTH_BUFFER_BIT);

							glUseProgram(_ShadowCubePass.getProgram());

							Mat4f shadowProj = glm::perspective(PI_2f, 1.f, 1e-3f, l.getRange());
							std::vector<Mat4f> shadowTransforms = {
								shadowProj * glm::lookAt(l.getInstance(i)->getPosition(), l.getInstance(i)->getPosition() + VEC3F_X, -VEC3F_Y),
								shadowProj * glm::lookAt(l.getInstance(i)->getPosition(), l.getInstance(i)->getPosition() - VEC3F_X, -VEC3F_Y),
								shadowProj * glm::lookAt(l.getInstance(i)->getPosition(), l.getInstance(i)->getPosition() + VEC3F_Y,  VEC3F_Z),
								shadowProj * glm::lookAt(l.getInstance(i)->getPosition(), l.getInstance(i)->getPosition() - VEC3F_Y, -VEC3F_Z),
								shadowProj * glm::lookAt(l.getInstance(i)->getPosition(), l.getInstance(i)->getPosition() + VEC3F_Z, -VEC3F_Y),
								shadowProj * glm::lookAt(l.getInstance(i)->getPosition(), l.getInstance(i)->getPosition() - VEC3F_Z, -VEC3F_Y)
							};

							glProgramUniformMatrix4fv(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uShadowTransform"), 6, false, glm::value_ptr(shadowTransforms[0]));
							glProgramUniform3fv(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uLightPos"), 1, glm::value_ptr(l.getInstance(i)->getPosition()));
							glProgramUniform1f(_ShadowCubePass.getProgram(), _ShadowCubePass.getUniform("uZfar"), l.getRange());

							for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
								for (unsigned int j = 0; j < mesh.first->getSubMeshes().size(); j++) {
									Scene::SubMesh subMesh = mesh.first->getSubMeshes()[j];
									
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
						}

						glViewport(0, 0, p_width, p_height);

						glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

						glEnable(GL_BLEND);
						glBlendFunc(GL_ONE, GL_ONE);

						glUseProgram(_DirectLightingPass.getProgram());

						glBindTextureUnit(0, p_albedoMap);
						glBindTextureUnit(1, p_normalMap);
						glBindTextureUnit(2, p_metalnessRoughnessMap);
						glBindTextureUnit(3, p_depthMap);
						glBindTextureUnit(4, _shadowMap);
						glBindTextureUnit(5, _shadowCubeMap);
						glBindImageTexture(6, p_rootTransparency, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
						glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, p_ssboTransparency);

						glProgramUniform3fv(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uCamPos"), 1, glm::value_ptr(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition()));
						glProgramUniformMatrix4fv(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uInvMatrix_VP"), 1, false, glm::value_ptr(glm::inverse(Application::getInstance().getSceneManager().getMainCameraProjectionMatrix() * Application::getInstance().getSceneManager().getMainCameraViewMatrix())));
						glProgramUniform1f(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uShadowFar"), l.getRange());
						glProgramUniformMatrix4fv(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uLightMatrix_VP"), 1, false, glm::value_ptr(homogeneousLightMatrix_VP));
						glProgramUniform3fv(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uLightPosition"), 1, glm::value_ptr(l.getInstance(i)->getPosition()));
						glProgramUniform3fv(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uLightDirection"), 1, glm::value_ptr(l.getInstance(i)->getFront()));
						glProgramUniform3fv(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uLightEmissivity"), 1, glm::value_ptr(l.getEmissivity()));
						glProgramUniform2fv(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uLightCosAngles"), 1, glm::value_ptr(Vec2f(l.getCosInnerConeAngle(), l.getCosOuterConeAngle())));
						glProgramUniform1i(_DirectLightingPass.getProgram(), _DirectLightingPass.getUniform("uLightTypePoint"), l.getType()!=LIGHT_TYPE::DIRECTIONAL);

						/* --- billboard full screen --- 
						  _billBoardCoords[0] = Vec3f(-1., -1., 0., 1.);
						  _billBoardCoords[1] = Vec3f( 1., -1., 0., 1.);
						  _billBoardCoords[2] = Vec3f(-1.,  1., 0., 1.);
						  _billBoardCoords[3] = Vec3f( 1.,  1., 0., 1.); */
						/* --- billboard light range ---
						  Mat4f p_matrixVP = Application::getInstance().getSceneManager().getMainCameraProjectionMatrix() * Application::getInstance().getSceneManager().getMainCameraViewMatrix();
						  
						  Vec4f a = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getUp() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getLeft()) * l.getRange() + lightPos, 1.f);
						  if (abs(a.a) > 0.01f) a /= a.a;

						  Vec4f b = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getUp() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getRight()) * l.getRange() + lightPos, 1.f);
						  if (abs(b.a) > 0.01f)  b /= b.a;

						  Vec4f c = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getDown() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getLeft()) * l.getRange() + lightPos, 1.f);
						  if (abs(c.a) > 0.01f)  c /= c.a;

						  Vec4f d = p_matrixVP * Vec4f((Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getDown() + Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getRight()) * l.getRange() + lightPos, 1.f);
						  if (abs(d.a) > 0.01f)  d /= d.a;

						  _billBoardCoords[0] = glm::clamp(Vec3f(a.x, a.y, 0.), -VEC3F_ONE, VEC3F_ONE);
						  _billBoardCoords[1] = glm::clamp(Vec3f(b.x, b.y, 0.), -VEC3F_ONE, VEC3F_ONE);
						  _billBoardCoords[2] = glm::clamp(Vec3f(c.x, c.y, 0.), -VEC3F_ONE, VEC3F_ONE);
						  _billBoardCoords[3] = glm::clamp(Vec3f(d.x, d.y, 0.), -VEC3F_ONE, VEC3F_ONE);*/
						// glNamedBufferSubData(_billboardVBO, 0, 4 * sizeof(Vec3f), &_billBoardCoords); // bind en 0 ?? 

						glBindVertexArray(_emptyVAO);
						glDrawArrays(GL_TRIANGLES, 0, 3);
						glBindVertexArray(0);

						glDisable(GL_BLEND);
						glBindFramebuffer(GL_FRAMEBUFFER, 0);
					}

				// --- indirect lighting ---
				glViewport(0, 0, p_width, p_height);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboLighting);

				glUseProgram(_IndirectLightingPass.getProgram());

				glProgramUniform3fv(_IndirectLightingPass.getProgram(), _IndirectLightingPass.getUniform("uCamPos"), 1, glm::value_ptr(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition()));

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);

				// --- Final Mix --- (rename apply transparent mask)
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

			GLuint _billBoardVBO		= GL_INVALID_INDEX;
			GLuint _billBoardVAO		= GL_INVALID_INDEX;
			GLuint _emptyVAO			= GL_INVALID_INDEX;

			unsigned int _shadowMapResolution = 1024;

			ProgramOGL _ShadowPass					= ProgramOGL("src/renderer/OpenGL/shaders/shadow/Shadow.vert", "", "src/renderer/OpenGL/shaders/shadow/Shadow.frag");
			ProgramOGL _ShadowCubePass				= ProgramOGL("src/renderer/OpenGL/shaders/shadow/CubeShadow.vert", "src/renderer/OpenGL/shaders/shadow/CubeShadow.geom", "src/renderer/OpenGL/shaders/shadow/CubeShadow.frag");
			//ProgramOGL _GenerateShadowMaps		= ProgramOGL("src/renderer/OpenGL/shaders/shadow/QuadScreen.vert", "src/renderer/OpenGL/shaders/shadow/GenerateShadowMaps.frag"); // cast shadow ray (indirect call compute)  

			ProgramOGL _DirectLightingPass			= ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/DirectLightingPass.frag");
			ProgramOGL _IndirectLightingPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/IndirectLightingPass.frag");
			ProgramOGL _FinalMixLightingPass		= ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/lighting/FinalMixLightingPass.frag");

		};
	}
}

#endif
