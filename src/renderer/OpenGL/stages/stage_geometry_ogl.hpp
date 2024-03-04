#ifndef __STAGE_GEOMETRY_OGL_HPP__
#define __STAGE_GEOMETRY_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"

#include "stage_ogl.hpp"

#include "application.hpp"
#include "renderer/renderer.hpp"
#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class StageGeometryOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageGeometryOGL() {
				_GeometryPass.addUniform("uAlbedo");
				_GeometryPass.addUniform("uEmissiveColor");
				_GeometryPass.addUniform("uEmissiveStrength");
				_GeometryPass.addUniform("uMetalness");
				_GeometryPass.addUniform("uRoughness");
				_GeometryPass.addUniform("uAlphaCutOff");

				_GeometryPass.addUniform("uHasAlbedoMap");
				_GeometryPass.addUniform("uHasMetalnessRoughnessMap");
				_GeometryPass.addUniform("uHasNormalMap");
				_GeometryPass.addUniform("uHasEmissiveMap");

				_GeometryPass.addUniform("uNbFragmentsMax");

				glCreateFramebuffers(1, &_fbo);
				generateMap(&_positionMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _positionMap, 0);
				generateMap(&_normalMetalnessMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _normalMetalnessMap, 1);
				generateMap(&_albedoRoughnessMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _albedoRoughnessMap, 2);
				generateMap(&_emissiveMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _emissiveMap, 3);
				generateMap(&_depthMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachDepthMap(_fbo, _depthMap);
				GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glNamedFramebufferDrawBuffers(_fbo, 4, DrawBuffers);

				generateMap(&_rootTransparency, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				glCreateBuffers(1, &_ssboTransparency);
				glCreateBuffers(1, &_counterTransparency);

				glNamedBufferStorage(_counterTransparency, sizeof(unsigned int), nullptr, GL_DYNAMIC_STORAGE_BIT);

				glCreateVertexArrays(1, &_emptyVAO);
			}

			~StageGeometryOGL() {
				glDeleteTextures(1, &_positionMap);
				glDeleteTextures(1, &_normalMetalnessMap);
				glDeleteTextures(1, &_albedoRoughnessMap);
				glDeleteTextures(1, &_emissiveMap);
				glDeleteTextures(1, &_depthMap);
				glDeleteFramebuffers(1, &_fbo);

				glDeleteTextures(1, &_rootTransparency);
				glDeleteBuffers(1, &_ssboTransparency);
				glDeleteBuffers(1, &_counterTransparency);

				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getPositionMap()		   { return _positionMap; }
			GLuint getNormalMetalnessMap() { return _normalMetalnessMap; }
			GLuint getAlbedoRoughnessMap() { return _albedoRoughnessMap; }
			GLuint getEmissiveMap()		   { return _emissiveMap; }
			GLuint getRootTransparency()   { return _rootTransparency; }
			GLuint getSSBOTransparency()   { return _ssboTransparency; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _positionMap);
				resizeColorMap(p_width, p_height, _normalMetalnessMap);
				resizeColorMap(p_width, p_height, _albedoRoughnessMap);
				resizeColorMap(p_width, p_height, _emissiveMap);
				resizeDepthMap(p_width, p_height, _depthMap);

				glBindTexture(GL_TEXTURE_2D, _rootTransparency);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, p_width, p_height, 0, GL_RED, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				_maxTransparentFragments = 25 * p_width * p_height;
				glDeleteBuffers(1, &_ssboTransparency);
				glCreateBuffers(1, &_ssboTransparency);
				glNamedBufferStorage(_ssboTransparency, _maxTransparentFragments * (16*sizeof(float)+sizeof(unsigned int)), nullptr, GL_DYNAMIC_STORAGE_BIT);
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures) {
				glViewport(0, 0, p_width, p_height);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_GeometryPass.getProgram());

				glClearTexImage(_rootTransparency,0,GL_RED,GL_UNSIGNED_INT,&_clearValue);
				glBindImageTexture(5,_rootTransparency,0,GL_FALSE,0,GL_READ_WRITE,GL_R32UI);

				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, _ssboTransparency);

				glNamedBufferSubData(_counterTransparency, 0, sizeof(unsigned int), &_clearValue);
				glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 7, _counterTransparency);

				glProgramUniform1ui(_GeometryPass.getProgram(), _GeometryPass.getUniform("uNbFragmentsMax"), _maxTransparentFragments);

				// TODO frustum culling
				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes) {
					for (unsigned int i = 0; i < mesh.first->getSubMeshes().size();i++) {
						Scene::SubMesh subMesh = mesh.first->getSubMeshes()[i];

						if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
						else { glDisable(GL_CULL_FACE); }

						glProgramUniform4fv(_GeometryPass.getProgram(), _GeometryPass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
						glProgramUniform1i(_GeometryPass.getProgram(), _GeometryPass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
						if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

						glProgramUniform1f(_GeometryPass.getProgram(), _GeometryPass.getUniform("uMetalness"), subMesh.getMaterial().getMetalness());
						glProgramUniform1f(_GeometryPass.getProgram(), _GeometryPass.getUniform("uRoughness"), subMesh.getMaterial().getRoughness());
						glProgramUniform1i(_GeometryPass.getProgram(), _GeometryPass.getUniform("uHasMetalnessRoughnessMap"), subMesh.getMaterial().getMetalnessRoughnessMap() != nullptr);
						if (subMesh.getMaterial().getMetalnessRoughnessMap() != nullptr) glBindTextureUnit(2, p_textures.at(subMesh.getMaterial().getMetalnessRoughnessMap())->getId());

						glProgramUniform1i(_GeometryPass.getProgram(), _GeometryPass.getUniform("uHasNormalMap"), subMesh.getMaterial().getNormalMap() != nullptr);
						if (subMesh.getMaterial().getNormalMap() != nullptr) glBindTextureUnit(3, p_textures.at(subMesh.getMaterial().getNormalMap())->getId());

						glProgramUniform3fv(_GeometryPass.getProgram(), _GeometryPass.getUniform("uEmissiveColor"), 1, glm::value_ptr(subMesh.getMaterial().getEmissiveColor()));
						glProgramUniform1f(_GeometryPass.getProgram(), _GeometryPass.getUniform("uEmissiveStrength"), subMesh.getMaterial().getEmissiveStrength());
						glProgramUniform1i(_GeometryPass.getProgram(), _GeometryPass.getUniform("uHasEmissiveMap"), subMesh.getMaterial().getEmissiveMap() != nullptr);
						if (subMesh.getMaterial().getEmissiveMap() != nullptr) glBindTextureUnit(4, p_textures.at(subMesh.getMaterial().getEmissiveMap())->getId());

						glProgramUniform1f(_GeometryPass.getProgram(), _GeometryPass.getUniform("uAlphaCutOff"), subMesh.getMaterial().getAlphaCutOff());

						mesh.second->bind(i);
						glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)subMesh.getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh.first->getNumberInstances());
						glBindVertexArray(0);
					}
				}

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glUseProgram(_FilterAndSortTranspFragsPass.getProgram());

				glBindTextureUnit(0, _depthMap);
				glBindImageTexture(1, _rootTransparency, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _ssboTransparency);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _emptyVAO = GL_INVALID_INDEX;
			
			GLuint _fbo = GL_INVALID_INDEX;

			unsigned int _maxTransparentFragments = 0;
			unsigned int _clearValue = 0;

			GLuint _rootTransparency = GL_INVALID_INDEX;
			GLuint _ssboTransparency = GL_INVALID_INDEX;
			GLuint _counterTransparency = GL_INVALID_INDEX;

			GLuint _positionMap = GL_INVALID_INDEX;
			GLuint _normalMetalnessMap = GL_INVALID_INDEX;
			GLuint _albedoRoughnessMap = GL_INVALID_INDEX;
			GLuint _emissiveMap = GL_INVALID_INDEX;
			GLuint _depthMap = GL_INVALID_INDEX;

			ProgramOGL _GeometryPass = ProgramOGL("src/renderer/OpenGL/shaders/geometry/GeometryPass.vert", "", "src/renderer/OpenGL/shaders/geometry/GeometryPass.frag");
			ProgramOGL _FilterAndSortTranspFragsPass = ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/geometry/FilterAndSortTranspFragsPass.frag");
		};
	}
}

#endif