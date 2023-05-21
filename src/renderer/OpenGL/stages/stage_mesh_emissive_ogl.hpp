#ifndef __STAGE_MESH_EMISSIVE_OGL_HPP__
#define __STAGE_MESH_EMISSIVE_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"

#include "stage_ogl.hpp"

#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class StageMeshEmissiveOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageMeshEmissiveOGL() {
				_emissivityPass.addUniform("uEmissivity");
				_emissivityPass.addUniform("uHasEmissivityMap");

				glCreateFramebuffers(1, &_fbo);
				generateMap(&_emissiveMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _emissiveMap, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fbo, 1, DrawBuffers);

				glCreateRenderbuffers(1, &_rbo);
				glNamedRenderbufferStorage(_rbo, GL_DEPTH_COMPONENT, 1, 1);
				glNamedFramebufferRenderbuffer(_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);
			}

			~StageMeshEmissiveOGL() {
				glDeleteTextures(1, &_emissiveMap);
				glDeleteRenderbuffers(1, &_rbo);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			inline GLuint getEmissiveMap() { return _emissiveMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _emissiveMap);

				resizeRbo(p_width, p_height, _rbo);
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures) {
				glViewport(0, 0, p_width, p_height);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_emissivityPass.getProgram());

				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes) {
					for (unsigned int i = 0; i < mesh.first->getPrimitives().size();i++) {
						Scene::Primitive* primitive = mesh.first->getPrimitives()[i];
						if (!primitive->getMaterial().isEmissive()) continue;

						glProgramUniform3fv(_emissivityPass.getProgram(), _emissivityPass.getUniform("uEmissivity"), 1, glm::value_ptr(primitive->getMaterial().getEmissivity()));
						glProgramUniform1i(_emissivityPass.getProgram(), _emissivityPass.getUniform("uHasEmissivityMap"), primitive->getMaterial().getEmissivityMap() != nullptr);
						if (primitive->getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(primitive->getMaterial().getEmissivityMap())->getId());

						mesh.second->bind(i);
						glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)primitive->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
						glBindVertexArray(0);
					}
				}

				glDisable(GL_DEPTH_TEST);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo = GL_INVALID_INDEX;
			GLuint _rbo = GL_INVALID_INDEX;

			GLuint _emissiveMap = GL_INVALID_INDEX;

			ProgramOGL _emissivityPass = ProgramOGL("src/renderer/OpenGL/shaders/.vert", "", "src/renderer/OpenGL/shaders/.frag");
		};
	}
}
#endif