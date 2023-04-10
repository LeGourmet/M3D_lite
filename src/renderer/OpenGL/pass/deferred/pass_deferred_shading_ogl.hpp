#ifndef __PASS_DEFERRED_SHADING_OGL_HPP__
#define __PASS_DEFERRED_SHADING_OGL_HPP__

#include "GL/gl3w.h"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "scene/objects/lights/light.hpp"

#include "program_geometry_ogl.hpp"
#include "program_lighting_directional.hpp"
#include "program_lighting_ponctual.hpp"
#include "program_shadow.hpp"
#include "program_shadow_cube.hpp"

namespace M3D
{
	namespace Renderer
	{
		class PassDeferredShadingOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			PassDeferredShadingOGL() {
				glCreateFramebuffers(1, &_fbo);

				glCreateTextures(GL_TEXTURE_2D, 1, &_shadingMap);
				glTextureParameteri(_shadingMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(_shadingMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _shadingMap, 0);

				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fbo, 1, DrawBuffers);
			}

			~PassDeferredShadingOGL() { }

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getShadingMap() { return _shadingMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) {
				_geometryPass.resize(p_width, p_height);
				_directionalLightingPass.resize(p_width, p_height);
				_shadowPass.resize(p_width, p_height);
				_ponctualLightingPass.resize(p_width, p_height);
				_cubeShadowPass.resize(p_width, p_height);
				
				glBindTexture(GL_TEXTURE_2D, _shadingMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_width, p_height);
			}

			void execute(std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Image*, TextureOGL*> p_textures) {
				_geometryPass.execute(p_meshes,p_textures);
				
				// Mat4f invMatrixCamVP = glm::inverse(Application::getInstance().getSceneManager().getMainCameraProjectionMatrix()) * glm::inverse(Application::getInstance().getSceneManager().getMainCameraViewMatrix());
				// compute frostrum => vector 
				// Scene::SceneGraphNode* p_instanceCamera, Scene::Camera* p_cam,

				for (Scene::Light* l : Application::getInstance().getSceneManager().getLights())
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
								float zfar = 100.;
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
								_shadowPass.execute(l->getInstance(i)->getPosition(), zfar, p_meshes);

								_directionalLightingPass.execute(l->getInstance(i), l, VEC3F_ZERO, zfar, _geometryPass.getPositionMap(), _geometryPass.getNormalMetalnessMap(), _geometryPass.getAlbedoRoughnessMap(), , _fbo);
								break;
						}
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo			= GL_INVALID_INDEX;
			GLuint _shadingMap	= GL_INVALID_INDEX;

			ProgramGeometryOGL				_geometryPass				= ProgramGeometryOGL();
			ProgramLightingDirectionalOGL	_directionalLightingPass	= ProgramLightingDirectionalOGL();
			ProgramLightingPonctualOGL		_ponctualLightingPass		= ProgramLightingPonctualOGL();
			ProgramShadowOGL				_shadowPass					= ProgramShadowOGL();
			ProgramShadowCubeOGL			_cubeShadowPass				= ProgramShadowCubeOGL();
		};
	}
}
#endif