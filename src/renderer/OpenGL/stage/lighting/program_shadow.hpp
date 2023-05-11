#ifndef __PROGRAM_SHADOW_OGL_HPP__
#define __PROGRAM_SHADOW_OGL_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "../program_ogl.hpp"

#include "utils/define.hpp"
#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class ProgramShadowOGL : public ProgramOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ProgramShadowOGL() : ProgramOGL("src/renderer/OpenGL/shaders/shading/shadowMapPass.vert", "src/renderer/OpenGL/shaders/shading/shadowMap.frag") {
				_uLightPosLoc	= glGetUniformLocation(_program, "uLightPos");
				_uZfarLoc		= glGetUniformLocation(_program, "uZfar");
				
				glCreateFramebuffers(1, &_fbo);

				glGenTextures(1, &_shadowMap);
				glBindTexture(GL_TEXTURE_2D, _shadowMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _swadowMapResolution, _swadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowMap, 0);
				glViewport(0, 0, _swadowMapResolution, _swadowMapResolution);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			~ProgramShadowOGL() {
				glDeleteTextures(1, &_shadowMap);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getShadowMap() { return _shadowMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override { }

			void execute(const Vec3f& p_lightPos, float p_zfar, const std::map<Scene::Mesh*, MeshOGL*>& p_meshes_ogl) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);

				glUseProgram(_program);

				glProgramUniform3fv(_program, _uLightPosLoc, 1, glm::value_ptr(p_lightPos));
				glProgramUniform1f(_program, _uZfarLoc, p_zfar);

				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes_ogl) {
					for (unsigned int i=0; i<mesh.first->getPrimitives().size() ;i++) {
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
			GLuint _fbo				= GL_INVALID_INDEX;

			GLuint _uLightPosLoc	= GL_INVALID_INDEX;
			GLuint _uZfarLoc		= GL_INVALID_INDEX;

			GLuint _shadowMap		= GL_INVALID_INDEX;
			unsigned int _swadowMapResolution = 1024;
		};
	}
}
#endif