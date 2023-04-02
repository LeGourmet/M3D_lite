#ifndef __CUBE_SHADOW_PASS_OGL_HPP__
#define __CUBE_SHADOW_PASS_OGL_HPP__

#include "pass_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class CubeShadowPassOGL : public PassOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			CubeShadowPassOGL(std::string p_pathVert, std::string p_pathFrag) : PassOGL(p_pathVert, p_pathFrag) {
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
				glViewport(0, 0, _swadowMapResolution, _swadowMapResolution);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			~CubeShadowPassOGL() {
				glDeleteTextures(1, &_cubeShadowMap);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getCubeShadowMap() { return _cubeShadowMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override { }

			void execute() {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glClear(GL_DEPTH_BUFFER_BIT);
				
				glUseProgram(_program);

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
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo = GL_INVALID_INDEX;

			unsigned int _swadowMapResolution = 1024;
			GLuint _cubeShadowMap = GL_INVALID_INDEX;
		};
	}
}
#endif