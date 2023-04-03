#ifndef __GEOMETRY_PASS_OGL_HPP__
#define __GEOMETRY_PASS_OGL_HPP__

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
		class GeometryPassOGL : public PassOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			GeometryPassOGL(std::string p_pathVert, std::string p_pathFrag) : PassOGL(p_pathVert, p_pathFrag) {
				_uAlbedoLoc						= glGetUniformLocation(_program, "uAlbedo");
				_uMetalnessLoc					= glGetUniformLocation(_program, "uMetalness");
				_uRoughnessLoc					= glGetUniformLocation(_program, "uRoughness");
				_uEmissivityLoc					= glGetUniformLocation(_program, "uEmissivity");

				_uHasAlbedoMapLoc				= glGetUniformLocation(_program, "uHasAlbedoMap");
				_uHasMetalnessRoughnessMapLoc	= glGetUniformLocation(_program, "uHasMetalnessRoughnessMap");
				_uHasNormalMapLoc				= glGetUniformLocation(_program, "uHasNormalMap");
				_uHasEmissivityMapLoc			= glGetUniformLocation(_program, "uHasEmissivityMap");

				glCreateFramebuffers(1, &_fbo);
				_generateAndAttachMap(_fbo, &_positionMap, 0);
				_generateAndAttachMap(_fbo, &_normalMetalnessMap, 1);
				_generateAndAttachMap(_fbo, &_albedoRoughnessMap, 2);
				_generateAndAttachMap(_fbo, &_emissivityMap, 3);
				GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glNamedFramebufferDrawBuffers(_fbo, 4, DrawBuffers);

				glCreateRenderbuffers(1, &_rbo);
				glNamedRenderbufferStorage(_rbo, GL_DEPTH_COMPONENT, 1, 1);
				glNamedFramebufferRenderbuffer(_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);
			}
			~GeometryPassOGL() {
				glDeleteTextures(1, &_positionMap);
				glDeleteTextures(1, &_normalMetalnessMap);
				glDeleteTextures(1, &_albedoRoughnessMap);
				glDeleteTextures(1, &_emissivityMap);
				glDeleteRenderbuffers(1, &_rbo);
				glDeleteFramebuffers(1, &_fbo);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getPositionMap() { return _positionMap; }
			GLuint getNormalMetalnessMap() { return _normalMetalnessMap; }
			GLuint getAlbedoRoughnessMap() { return _albedoRoughnessMap; }
			GLuint getEmissivityMap() { return _emissivityMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override {
				glBindTexture(GL_TEXTURE_2D, _positionMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _normalMetalnessMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _albedoRoughnessMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _emissivityMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_width, p_height);

				glNamedRenderbufferStorage(_rbo, GL_DEPTH_COMPONENT, p_width, p_height);
			}

			void execute(const std::map<Scene::Mesh*, MeshOGL*>& p_meshes_ogl, const std::map<Image*, TextureOGL*>& p_textures_ogl) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_program);

				for (std::pair<Scene::Mesh*, MeshOGL*> mesh : p_meshes_ogl) {
					for (unsigned int i = 0; i < mesh.first->getPrimitives().size();i++) {
						Scene::Primitive* primitive = mesh.first->getPrimitives()[i];
						if (primitive->getMaterial().isTransparent()) continue;

						glProgramUniform1i(_program, _uHasAlbedoMapLoc, primitive->getMaterial().getBaseColorMap() != nullptr);
						glProgramUniform4fv(_program, _uAlbedoLoc, 1, glm::value_ptr(primitive->getMaterial().getBaseColor()));
						if (primitive->getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(1, p_textures_ogl.at(primitive->getMaterial().getBaseColorMap())->getId());

						glProgramUniform1i(_program, _uHasMetalnessRoughnessMapLoc, primitive->getMaterial().getMetalnessRoughnessMap() != nullptr);
						glProgramUniform1f(_program, _uMetalnessLoc, primitive->getMaterial().getMetalness());
						glProgramUniform1f(_program, _uRoughnessLoc, primitive->getMaterial().getRoughness());
						if (primitive->getMaterial().getMetalnessRoughnessMap() != nullptr) glBindTextureUnit(2, p_textures_ogl.at(primitive->getMaterial().getMetalnessRoughnessMap())->getId());

						glProgramUniform1i(_program, _uHasNormalMapLoc, primitive->getMaterial().getNormalMap() != nullptr);
						if (primitive->getMaterial().getNormalMap() != nullptr) glBindTextureUnit(3, p_textures_ogl.at(primitive->getMaterial().getNormalMap())->getId());

						glProgramUniform1i(_program, _uHasEmissivityMapLoc, primitive->getMaterial().getEmissivityMap() != nullptr);
						glProgramUniform3fv(_program, _uEmissivityLoc, 1, glm::value_ptr(primitive->getMaterial().getEmissivity()));
						if (primitive->getMaterial().getEmissivityMap() != nullptr) glBindTextureUnit(4, p_textures_ogl.at(primitive->getMaterial().getEmissivityMap())->getId());

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
			GLuint _rbo = GL_INVALID_INDEX;

			GLuint _positionMap = GL_INVALID_INDEX;
			GLuint _normalMetalnessMap = GL_INVALID_INDEX;
			GLuint _albedoRoughnessMap = GL_INVALID_INDEX;
			GLuint _emissivityMap = GL_INVALID_INDEX;

			GLint  _uAlbedoLoc = GL_INVALID_INDEX;
			GLint  _uMetalnessLoc = GL_INVALID_INDEX;
			GLint  _uRoughnessLoc = GL_INVALID_INDEX;
			GLint  _uEmissivityLoc = GL_INVALID_INDEX;

			GLint  _uHasAlbedoMapLoc = GL_INVALID_INDEX;
			GLint  _uHasMetalnessRoughnessMapLoc = GL_INVALID_INDEX;
			GLint  _uHasNormalMapLoc = GL_INVALID_INDEX;
			GLint  _uHasEmissivityMapLoc = GL_INVALID_INDEX;
		};
	}
}
#endif