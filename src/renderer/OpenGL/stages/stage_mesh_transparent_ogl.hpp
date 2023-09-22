#ifndef __STAGE_MESH_TRANSPARENT_OGL_HPP__
#define __STAGE_MESH_TRANSPARENT_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"

#include "stage_ogl.hpp"

#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

#include <iostream>

namespace M3D
{
	namespace Renderer
	{
		class StageMeshTransparentOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageMeshTransparentOGL() { 
				_transparencyPass.addUniform("uAlbedo");
				_transparencyPass.addUniform("uEmissiveColor");
				_transparencyPass.addUniform("uEmissiveStrength");
				_transparencyPass.addUniform("uAlphaCutOff");
				_transparencyPass.addUniform("uHasAlbedoMap");
				_transparencyPass.addUniform("uHasEmissiveMap");

				glCreateFramebuffers(1, &_fbo);
				generateMap(&_transparencyMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _transparencyMap, 0);
				generateMap(&_depthMap, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
				attachDepthMap(_fbo, _depthMap); // todo add stencil
			
				generateMap(&_layerMap, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
			}

			~StageMeshTransparentOGL() { 
				glDeleteTextures(1, &_transparencyMap);
				glDeleteTextures(1, &_layerMap);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getTransparencyMap() { return _transparencyMap; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _transparencyMap);
				resizeDepthMap(p_width, p_height, _depthMap);
				resizeDepthMap(p_width, p_height, _layerMap);
			}

			// todo frutum culling 
			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_depthMapOpaque) {
				glViewport(0, 0, p_width, p_height);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_GREATER);
				glClearColor(1.f,1.f,1.f,1.f);
				glClearDepth(0.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_transparencyPass.getProgram());

				glCopyImageSubData(p_depthMapOpaque, GL_TEXTURE_2D, 0, 0, 0, 0, _layerMap, GL_TEXTURE_2D, 0, 0, 0, 0, p_width, p_height, 1);
				glBindTextureUnit(3, _layerMap);
				
				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
					for (unsigned int i=0; i<mesh.first->getSubMeshes().size() ;i++) {
						Scene::SubMesh subMesh = mesh.first->getSubMeshes()[i];
						if (!subMesh.getMaterial().isTransparent()) continue;

						if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
						else { glDisable(GL_CULL_FACE); }

						glProgramUniform4fv(_transparencyPass.getProgram(), _transparencyPass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
						glProgramUniform1i(_transparencyPass.getProgram(), _transparencyPass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
						if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

						glProgramUniform3fv(_transparencyPass.getProgram(), _transparencyPass.getUniform("uEmissiveColor"), 1, glm::value_ptr(subMesh.getMaterial().getEmissiveColor()));
						glProgramUniform1f(_transparencyPass.getProgram(), _transparencyPass.getUniform("uEmissiveStrength"), subMesh.getMaterial().getEmissiveStrength());
						glProgramUniform1i(_transparencyPass.getProgram(), _transparencyPass.getUniform("uHasEmissiveMap"), subMesh.getMaterial().getEmissiveMap() != nullptr);
						if (subMesh.getMaterial().getEmissiveMap() != nullptr) glBindTextureUnit(2, p_textures.at(subMesh.getMaterial().getEmissiveMap())->getId());

						glProgramUniform1f(_transparencyPass.getProgram(), _transparencyPass.getUniform("uAlphaCutOff"), subMesh.getMaterial().getAlphaCutOff());

						mesh.second->bind(i);
						glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)subMesh.getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
						glBindVertexArray(0);
					}

				// check stencil value
				int stencilDepth = 1;

				for(int i=1; i<stencilDepth ;i++){
					glCopyImageSubData(_depthMap, GL_TEXTURE_2D, 0, 0, 0, 0, _layerMap, GL_TEXTURE_2D, 0, 0, 0, 0, p_width, p_height, 1);

					glClear(GL_DEPTH_BUFFER_BIT);
					
					for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes)
						for (unsigned int j=0; j<mesh.first->getSubMeshes().size(); j++) {
							Scene::SubMesh subMesh = mesh.first->getSubMeshes()[j];
							if (!subMesh.getMaterial().isTransparent()) continue;

							if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
							else { glDisable(GL_CULL_FACE); }

							glProgramUniform4fv(_transparencyPass.getProgram(), _transparencyPass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
							glProgramUniform1i(_transparencyPass.getProgram(), _transparencyPass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
							if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

							glProgramUniform3fv(_transparencyPass.getProgram(), _transparencyPass.getUniform("uEmissiveColor"), 1, glm::value_ptr(subMesh.getMaterial().getEmissiveColor()));
							glProgramUniform1f(_transparencyPass.getProgram(), _transparencyPass.getUniform("uEmissiveStrength"), subMesh.getMaterial().getEmissiveStrength());
							glProgramUniform1i(_transparencyPass.getProgram(), _transparencyPass.getUniform("uHasEmissiveMap"), subMesh.getMaterial().getEmissiveMap() != nullptr);
							if (subMesh.getMaterial().getEmissiveMap() != nullptr) glBindTextureUnit(2, p_textures.at(subMesh.getMaterial().getEmissiveMap())->getId());

							glProgramUniform1f(_transparencyPass.getProgram(), _transparencyPass.getUniform("uAlphaCutOff"), subMesh.getMaterial().getAlphaCutOff());

							mesh.second->bind(j);
							glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)subMesh.getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
							glBindVertexArray(0);
						}
				}

				glDisable(GL_CULL_FACE);
				glDepthFunc(GL_LESS);
				glClearDepth(1.f);
				glDisable(GL_DEPTH_TEST);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo				= GL_INVALID_INDEX;

			GLuint _transparencyMap = GL_INVALID_INDEX;
			GLuint _depthMap		= GL_INVALID_INDEX;
			GLuint _layerMap		= GL_INVALID_INDEX;

			ProgramOGL _transparencyPass = ProgramOGL("src/renderer/OpenGL/shaders/geometry/transparentPass.vert", "", "src/renderer/OpenGL/shaders/geometry/transparentPass.frag");
		};
	}
}

#endif
