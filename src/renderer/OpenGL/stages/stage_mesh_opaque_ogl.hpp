#ifndef __STAGE_MESH_OPAQUE_OGL_HPP__
#define __STAGE_MESH_OPAQUE_OGL_HPP__

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
		class StageMeshOpaqueOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageMeshOpaqueOGL() {
				_geometryPass.addUniform("uAlbedo");
				_geometryPass.addUniform("uEmissiveColor");
				_geometryPass.addUniform("uEmissiveStrength");
				_geometryPass.addUniform("uMetalness");
				_geometryPass.addUniform("uRoughness");
				_geometryPass.addUniform("uAlphaCutOff");

				_geometryPass.addUniform("uHasAlbedoMap");
				_geometryPass.addUniform("uHasMetalnessRoughnessMap");
				_geometryPass.addUniform("uHasNormalMap");
				_geometryPass.addUniform("uHasEmissiveMap");
				_geometryPass.addUniform("uHasOcclusionMap");

				glCreateFramebuffers(1, &_fbo);
				generateMap(&_positionMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _positionMap, 0);
				generateMap(&_normalMetalnessMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _normalMetalnessMap, 1);
				generateMap(&_albedoRoughnessMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _albedoRoughnessMap, 2);
				generateMap(&_emissiveAmbientMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _emissiveAmbientMap, 3);
				GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glNamedFramebufferDrawBuffers(_fbo, 4, DrawBuffers);

				glCreateRenderbuffers(1, &_rbo);
				glNamedRenderbufferStorage(_rbo, GL_DEPTH_COMPONENT, 1, 1);
				glNamedFramebufferRenderbuffer(_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);
			}

			~StageMeshOpaqueOGL() {
				glDeleteTextures(1, &_positionMap);
				glDeleteTextures(1, &_normalMetalnessMap);
				glDeleteTextures(1, &_albedoRoughnessMap);
				glDeleteTextures(1, &_emissiveAmbientMap);
				glDeleteRenderbuffers(1, &_rbo);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getPositionMap() { return _positionMap; }
			GLuint getNormalMetalnessMap() { return _normalMetalnessMap; }
			GLuint getAlbedoRoughnessMap() { return _albedoRoughnessMap; }
			GLuint getEmissiveAmbientMap() { return _emissiveAmbientMap; }
			GLuint getFBO() { return _fbo; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _positionMap);
				resizeColorMap(p_width, p_height, _normalMetalnessMap);
				resizeColorMap(p_width, p_height, _albedoRoughnessMap);
				resizeColorMap(p_width, p_height, _emissiveAmbientMap);

				resizeRbo(p_width, p_height, _rbo);
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures) {
				glViewport(0, 0, p_width, p_height);
				
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_geometryPass.getProgram());

				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes) {
					for (unsigned int i = 0; i < mesh.first->getPrimitives().size();i++) {
						Scene::Primitive* primitive = mesh.first->getPrimitives()[i];
						if (!primitive->getMaterial().isOpaque()) continue;

						glDisable(GL_CULL_FACE);
						if(primitive->getMaterial().isDoubleSide()){ glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }

						glProgramUniform4fv(_geometryPass.getProgram(), _geometryPass.getUniform("uAlbedo"), 1, glm::value_ptr(primitive->getMaterial().getBaseColor()));
						glProgramUniform1i(_geometryPass.getProgram(), _geometryPass.getUniform("uHasAlbedoMap"), primitive->getMaterial().getBaseColorMap() != nullptr);
						if (primitive->getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(primitive->getMaterial().getBaseColorMap())->getId());

						glProgramUniform1f(_geometryPass.getProgram(), _geometryPass.getUniform("uMetalness"), primitive->getMaterial().getMetalness());
						glProgramUniform1f(_geometryPass.getProgram(), _geometryPass.getUniform("uRoughness"), primitive->getMaterial().getRoughness());
						glProgramUniform1i(_geometryPass.getProgram(), _geometryPass.getUniform("uHasMetalnessRoughnessMap"), primitive->getMaterial().getMetalnessRoughnessMap() != nullptr);
						if (primitive->getMaterial().getMetalnessRoughnessMap() != nullptr) glBindTextureUnit(2, p_textures.at(primitive->getMaterial().getMetalnessRoughnessMap())->getId());

						glProgramUniform1i(_geometryPass.getProgram(), _geometryPass.getUniform("uHasNormalMap"), primitive->getMaterial().getNormalMap() != nullptr);
						if (primitive->getMaterial().getNormalMap() != nullptr) glBindTextureUnit(3, p_textures.at(primitive->getMaterial().getNormalMap())->getId());
						
						glProgramUniform3fv(_geometryPass.getProgram(), _geometryPass.getUniform("uEmissiveColor"), 1, glm::value_ptr(primitive->getMaterial().getEmissiveColor()));
						glProgramUniform1f(_geometryPass.getProgram(), _geometryPass.getUniform("uEmissiveStrength"), primitive->getMaterial().getEmissiveStrength());
						glProgramUniform1i(_geometryPass.getProgram(), _geometryPass.getUniform("uHasEmissiveMap"), primitive->getMaterial().getEmissiveMap() != nullptr);
						if (primitive->getMaterial().getEmissiveMap() != nullptr) glBindTextureUnit(4, p_textures.at(primitive->getMaterial().getEmissiveMap())->getId());

						glProgramUniform1i(_geometryPass.getProgram(), _geometryPass.getUniform("uHasOcclusionMap"), primitive->getMaterial().getOcclusionMap() != nullptr);
						if (primitive->getMaterial().getOcclusionMap() != nullptr) glBindTextureUnit(5, p_textures.at(primitive->getMaterial().getOcclusionMap())->getId());

						glProgramUniform1f(_geometryPass.getProgram(), _geometryPass.getUniform("uAlphaCutOff"), primitive->getMaterial().getAlphaCutOff());

						mesh.second->bind(i);
						glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)primitive->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
						glBindVertexArray(0);
					}
				}

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo					= GL_INVALID_INDEX;
			GLuint _rbo					= GL_INVALID_INDEX;

			GLuint _positionMap			= GL_INVALID_INDEX;
			GLuint _normalMetalnessMap	= GL_INVALID_INDEX;
			GLuint _albedoRoughnessMap	= GL_INVALID_INDEX;
			GLuint _emissiveAmbientMap  = GL_INVALID_INDEX;

			ProgramOGL _geometryPass = ProgramOGL("src/renderer/OpenGL/shaders/geometryPass.vert", "", "src/renderer/OpenGL/shaders/geometryPass.frag");
		};
	}
}
#endif