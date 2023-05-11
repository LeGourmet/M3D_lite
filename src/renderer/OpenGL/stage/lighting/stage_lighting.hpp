#ifndef __STAGE_LIGHTING_OGL_HPP__
#define __STAGE_LIGHTING_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"

#include "../stage_ogl.hpp"

#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class StageLighting : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageLighting() {
				_directionalLighting.addUniform("uCamDataPos");
				_directionalLighting.addUniform("uLightPosition");
				_directionalLighting.addUniform("uLightDirection");
				_directionalLighting.addUniform("uLightEmissivity");
				
				_ponctualLighting.addUniform("uCamData");
				_ponctualLighting.addUniform("uLightPosition");
				_ponctualLighting.addUniform("uLightDirection");
				_ponctualLighting.addUniform("uLightEmissivity");
				_ponctualLighting.addUniform("uLightCosAngles");
				
				_shadowCube.addUniform("uShadowTransform");
				_shadowCube.addUniform("uLightPos");
				_shadowCube.addUniform("uZfar");

				_shadow.addUniform("uLightPos");
				_shadow.addUniform("uZfar");
			}

			~StageLighting() { }

			// ----------------------------------------------------- GETTERS -------------------------------------------------------

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) {

			}

			void execute(std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Image*, TextureOGL*> p_textures) {
				// Mat4f invMatrixCamVP = glm::inverse(Application::getInstance().getSceneManager().getMainCameraProjectionMatrix()) * glm::inverse(Application::getInstance().getSceneManager().getMainCameraViewMatrix());
				// compute frostrum => vector 
				// Scene::SceneGraphNode* p_instanceCamera, Scene::Camera* p_cam,

				/*for (Scene::Light* l : Application::getInstance().getSceneManager().getLights())
					for (unsigned int i=0; i<l->getNumberInstances() ;i++)
						switch (l->getType()) {
							case LIGHT_TYPE::POINT:
							case LIGHT_TYPE::SPOT:
								float zfar = 100.;
								// frustrum => compare with aabb and just display inside frustrum || light frustrum
								_cubeShadowPass.execute(l->getInstance(i)->getPosition(), zfar, p_meshes);

								_ponctualLightingPass.execute( l->getInstance(i), l, zfar, _geometryPass.getPositionMap(), _geometryPass.getNormalMetalnessMap(), _geometryPass.getAlbedoRoughnessMap(), , _fbo);
								break;
							case LIGHT_TYPE::DIRECTIONAL:
								float zfar = 100.;*/
								/*
									float near_plane = 1.0f, far_plane = 7.5f;
									glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
									glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
												  glm::vec3( 0.0f, 0.0f,  0.0f),
												  glm::vec3( 0.0f, 1.0f,  0.0f));
									glm::mat4 lightSpaceMatrix = lightProjection * lightView;
									glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

									render all the scene
								*/

								// frustrum => compare with aabb and just display inside frustrum || light frustrum
								// Vec3f lightPos = VEC3F_ZERO; // compute position
								/*_shadowPass.execute(l->getInstance(i)->getPosition(), zfar, p_meshes);

								_directionalLightingPass.execute(l->getInstance(i), l, VEC3F_ZERO, zfar, _geometryPass.getPositionMap(), _geometryPass.getNormalMetalnessMap(), _geometryPass.getAlbedoRoughnessMap(), , _fbo);
								break;
						}*/
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			ProgramOGL _directionalLighting = ProgramOGL("src/renderer/shaders/utils/quadScreen.vert", "", "src/renderer/shaders/lighting/directionalLightingPass.frag");
			ProgramOGL _ponctualLighting	= ProgramOGL("src/renderer/shaders/utils/billboard.vert", "", "src/renderer/shaders/lighting/ponctualLightingPass.frag");
			ProgramOGL _shadow				= ProgramOGL("src/renderer/shaders/shadow/shadow.vert", "", "");
			ProgramOGL _shadowCube			= ProgramOGL("src/renderer/shaders/shadow/cubeShadow.vert", "src/renderer/shaders/shadow/shadow.geom", "src/renderer/shaders/shadow/shadow.frag");
		};
	}
}
#endif