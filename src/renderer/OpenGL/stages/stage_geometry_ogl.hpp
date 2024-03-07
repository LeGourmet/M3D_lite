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
				_GeometryPass.addUniform("uMetalness");
				_GeometryPass.addUniform("uRoughness");
				_GeometryPass.addUniform("uEmissiveColor");
				_GeometryPass.addUniform("uEmissiveStrength");
				_GeometryPass.addUniform("uAlphaCutOff");

				_GeometryPass.addUniform("uHasAlbedoMap");
				_GeometryPass.addUniform("uHasNormalMap");
				_GeometryPass.addUniform("uHasMetalnessRoughnessMap");
				_GeometryPass.addUniform("uHasEmissiveMap");

				_GeometryPass.addUniform("uNbTranspFragsMax");

				glCreateFramebuffers(1, &_fbo);
				generateMap(&_albedoMap, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				attachColorMap(_fbo, _albedoMap, 0);
				generateMap(&_normalMap, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				attachColorMap(_fbo, _normalMap, 1);
				generateMap(&_metalnessRoughnessMap, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				attachColorMap(_fbo, _metalnessRoughnessMap, 2);
				generateMap(&_emissiveMap, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				attachColorMap(_fbo, _emissiveMap, 3);
				generateMap(&_depthMap, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				attachDepthMap(_fbo, _depthMap);
				GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glNamedFramebufferDrawBuffers(_fbo, 4, DrawBuffers);

				generateMap(&_rootTransparency, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				glCreateBuffers(1, &_ssboTransparency);
				glCreateBuffers(1, &_counterTransparency);

				glNamedBufferStorage(_counterTransparency, sizeof(unsigned int), nullptr, GL_DYNAMIC_STORAGE_BIT);

				glCreateVertexArrays(1, &_emptyVAO);
			}

			~StageGeometryOGL() {
				glDeleteTextures(1, &_albedoMap);
				glDeleteTextures(1, &_normalMap);
				glDeleteTextures(1, &_metalnessRoughnessMap);
				glDeleteTextures(1, &_emissiveMap);
				glDeleteTextures(1, &_depthMap);
				glDeleteFramebuffers(1, &_fbo);

				glDeleteTextures(1, &_rootTransparency);
				glDeleteBuffers(1, &_ssboTransparency);
				glDeleteBuffers(1, &_counterTransparency);

				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getAlbedoMap()				{ return _albedoMap; }
			GLuint getNormalMap()				{ return _normalMap; }
			GLuint getMetalnessRoughnessMap()	{ return _metalnessRoughnessMap; }
			GLuint getEmissiveMap()				{ return _emissiveMap; }
			GLuint getDepthMap()				{ return _depthMap; }
			GLuint getRootTransparency()		{ return _rootTransparency; }
			GLuint getSSBOTransparency()		{ return _ssboTransparency; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(GL_RGB32F, GL_RGB, GL_FLOAT, p_width, p_height, _albedoMap);				//=> RGB_8 => unsigned
				resizeColorMap(GL_RGB32F, GL_RGB, GL_FLOAT, p_width, p_height, _normalMap);				//=> RGB_16 => convert  [-1;1] [0;1]/[0;2]
				resizeColorMap(GL_RG32F , GL_RG , GL_FLOAT, p_width, p_height, _metalnessRoughnessMap);	//=> RG_8   => unsigned
				resizeColorMap(GL_RGB32F, GL_RGB, GL_FLOAT, p_width, p_height, _emissiveMap);			//=> RGB_16 => unsigned 
				resizeDepthMap(p_width, p_height, _depthMap);
				
				glBindTexture(GL_TEXTURE_2D, _rootTransparency);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, p_width, p_height, 0, GL_RED, GL_FLOAT, 0); // should be 32ui ?
				glBindTexture(GL_TEXTURE_2D, 0);

				_maxTransparentFragments = 10 * p_width * p_height;
				glDeleteBuffers(1, &_ssboTransparency);
				glCreateBuffers(1, &_ssboTransparency);
				glNamedBufferStorage(_ssboTransparency, _maxTransparentFragments * (13*sizeof(float)+sizeof(unsigned int)), nullptr, GL_DYNAMIC_STORAGE_BIT);
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

				glProgramUniform1ui(_GeometryPass.getProgram(), _GeometryPass.getUniform("uNbTranspFragsMax"), _maxTransparentFragments);

				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes) {
					// todo call gpu bvh scene for instance frustum culling
					for (unsigned int i = 0; i < mesh.first->getSubMeshes().size();i++) {
						Scene::SubMesh subMesh = mesh.first->getSubMeshes()[i];

						if (subMesh.getMaterial().isDoubleSide()) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
						else { glDisable(GL_CULL_FACE); }

						glProgramUniform4fv(_GeometryPass.getProgram(), _GeometryPass.getUniform("uAlbedo"), 1, glm::value_ptr(subMesh.getMaterial().getBaseColor()));
						glProgramUniform1i(_GeometryPass.getProgram(), _GeometryPass.getUniform("uHasAlbedoMap"), subMesh.getMaterial().getBaseColorMap() != nullptr);
						if (subMesh.getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures.at(subMesh.getMaterial().getBaseColorMap())->getId());

						glProgramUniform1i(_GeometryPass.getProgram(), _GeometryPass.getUniform("uHasNormalMap"), subMesh.getMaterial().getNormalMap() != nullptr);
						if (subMesh.getMaterial().getNormalMap() != nullptr) glBindTextureUnit(2, p_textures.at(subMesh.getMaterial().getNormalMap())->getId());

						glProgramUniform1f(_GeometryPass.getProgram(), _GeometryPass.getUniform("uMetalness"), subMesh.getMaterial().getMetalness());
						glProgramUniform1f(_GeometryPass.getProgram(), _GeometryPass.getUniform("uRoughness"), subMesh.getMaterial().getRoughness());
						glProgramUniform1i(_GeometryPass.getProgram(), _GeometryPass.getUniform("uHasMetalnessRoughnessMap"), subMesh.getMaterial().getMetalnessRoughnessMap() != nullptr);
						if (subMesh.getMaterial().getMetalnessRoughnessMap() != nullptr) glBindTextureUnit(3, p_textures.at(subMesh.getMaterial().getMetalnessRoughnessMap())->getId());

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
			GLuint _emptyVAO							= GL_INVALID_INDEX;
			
			unsigned int _maxTransparentFragments		= 0;
			unsigned int _clearValue					= 0;

			GLuint _albedoMap							= GL_INVALID_INDEX;
			GLuint _normalMap							= GL_INVALID_INDEX;
			GLuint _metalnessRoughnessMap				= GL_INVALID_INDEX;
			GLuint _emissiveMap							= GL_INVALID_INDEX; 
			GLuint _depthMap							= GL_INVALID_INDEX;
			GLuint _rootTransparency					= GL_INVALID_INDEX;
			GLuint _ssboTransparency					= GL_INVALID_INDEX;
			GLuint _counterTransparency					= GL_INVALID_INDEX;

			GLuint _fbo									= GL_INVALID_INDEX;
			
			ProgramOGL _GeometryPass					= ProgramOGL("src/renderer/OpenGL/shaders/geometry/GeometryPass.vert", "", "src/renderer/OpenGL/shaders/geometry/GeometryPass.frag");
			ProgramOGL _FilterAndSortTranspFragsPass	= ProgramOGL("src/renderer/OpenGL/shaders/utils/QuadScreen.vert", "", "src/renderer/OpenGL/shaders/geometry/FilterAndSortTranspFrags.frag");
		};
	}
}

#endif
