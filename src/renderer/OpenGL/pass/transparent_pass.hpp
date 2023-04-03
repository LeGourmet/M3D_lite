#ifndef __TRANSPARENT_PASS_OGL_HPP__
#define __TRANSPARENT_PASS_OGL_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "pass_ogl.hpp"

#include "scene/objects/meshes/mesh.hpp"
#include "scene/objects/meshes/primitive.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"

#include <map>

namespace M3D
{
	namespace Renderer
	{
		class TransparentPassOGL : public PassOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			TransparentPassOGL(std::string p_pathVert, std::string p_pathFrag) : PassOGL(p_pathVert, p_pathFrag) {
				glCreateFramebuffers(1, &_fbo);
				_generateAndAttachMap(_fbo, &_transparentMap, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fbo, 4, DrawBuffers);
			}
			~TransparentPassOGL() {
				glDeleteTextures(1, &_transparentMap);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getTransparentMap() { return _transparentMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override {
				glBindTexture(GL_TEXTURE_2D, _transparentMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_width, p_height);
			}

			void execute(const std::map<Scene::Mesh*, MeshOGL*>& p_meshes_ogl, const std::map<Image*, TextureOGL*>& p_textures_ogl) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_program);

				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes_ogl) {
					for (unsigned int i = 0; i < mesh.first->getPrimitives().size();i++) {
						Scene::Primitive* primitive = mesh.first->getPrimitives()[i];
						if (primitive->getMaterial().isOpaque()) continue;

						mesh.second->bind(i);
						glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)primitive->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
						glBindVertexArray(0);
					}
				}

				glDisable(GL_DEPTH_TEST);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo = GL_INVALID_INDEX;

			GLuint _transparentMap = GL_INVALID_INDEX;
		};
	}
}
#endif
