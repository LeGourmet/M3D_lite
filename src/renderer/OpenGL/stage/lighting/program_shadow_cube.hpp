#ifndef __PROGRAM_SHADOW_CUBE_OGL_HPP__
#define __PROGRAM_SHADOW_CUBE_OGL_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "../program_ogl.hpp"

#include "utils/define.hpp"
#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class ProgramShadowCubeOGL : public ProgramOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ProgramShadowCubeOGL() : ProgramOGL("src/renderer/OpenGL/shaders/shading/shadowMapPass.vert", "src/renderer/OpenGL/shaders/shading/cubeShadowMapPass.geom", "src/renderer/OpenGL/shaders/shading/shadowMapPass.frag") {
				_uShadowTransformLoc	= glGetUniformLocation(_program, "uShadowTransform");
				_uLightPosLoc			= glGetUniformLocation(_program, "uLightPos");
				_uZfarLoc				= glGetUniformLocation(_program, "uZfar");

				glCreateFramebuffers(1, &_fbo);

				glGenTextures(1, &_cubeShadowMap);
				glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeShadowMap);
				for (unsigned int i = 0; i < 6;i++)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, _swadowMapResolution, _swadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _cubeShadowMap, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glViewport(0, 0, _swadowMapResolution, _swadowMapResolution);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			~ProgramShadowCubeOGL() {
				glDeleteTextures(1, &_cubeShadowMap);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getCubeShadowMap() { return _cubeShadowMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override { }

			void execute(const Vec3f& p_lightPos, float p_zfar, const std::map<Scene::Mesh*, MeshOGL*>& p_meshes_ogl) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);
				
				glUseProgram(_program);
				
				Mat4f shadowProj = glm::perspective(PIf*0.5f, 1.f, 1e-2f, p_zfar);
				std::vector<Mat4f> shadowTransforms = {
					shadowProj * glm::lookAt(p_lightPos, p_lightPos+VEC3F_X, -VEC3F_Y),
					shadowProj * glm::lookAt(p_lightPos, p_lightPos-VEC3F_X, -VEC3F_Y),
					shadowProj * glm::lookAt(p_lightPos, p_lightPos+VEC3F_Y,  VEC3F_Z),
					shadowProj * glm::lookAt(p_lightPos, p_lightPos-VEC3F_Y, -VEC3F_Z),
					shadowProj * glm::lookAt(p_lightPos, p_lightPos+VEC3F_Z, -VEC3F_Y),
					shadowProj * glm::lookAt(p_lightPos, p_lightPos-VEC3F_Z, -VEC3F_Y)
				};
				
				glProgramUniformMatrix4fv(_program, _uShadowTransformLoc, 6, false, glm::value_ptr(shadowTransforms[0]));
				glProgramUniform3fv(_program, _uLightPosLoc, 1, glm::value_ptr(p_lightPos));
				glProgramUniform1f(_program, _uZfarLoc, p_zfar);

				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes_ogl) {
					for (unsigned int i = 0; i < mesh.first->getPrimitives().size();i++) {
						Scene::Primitive* primitive = mesh.first->getPrimitives()[i];
						if (primitive->getMaterial().isTransparent()) continue;

						mesh.second->bind(i);
						glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)primitive->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
						glBindVertexArray(0);
					}
				}

				glDisable(GL_DEPTH_TEST);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo					= GL_INVALID_INDEX;

			GLuint _uShadowTransformLoc = GL_INVALID_INDEX;
			GLuint _uLightPosLoc		= GL_INVALID_INDEX;
			GLuint _uZfarLoc			= GL_INVALID_INDEX;

			GLuint _cubeShadowMap		= GL_INVALID_INDEX;
			unsigned int _swadowMapResolution = 1024;
		};
	}
}
#endif