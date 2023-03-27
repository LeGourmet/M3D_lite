#ifndef __GEOMETRY_PASS_OGL_HPP__
#define __GEOMETRY_PASS_OGL_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "pass_ogl.hpp"
#include "application.hpp"
#include "scene/scene_manager.hpp"

#include <map>

namespace M3D
{
namespace Renderer
{
	class GeometryPassOGL : public PassOGL{
		public:
			GeometryPassOGL(std::string p_pathVert, std::string p_pathFrag) : PassOGL(p_pathVert,p_pathFrag) {
				_uAlbedoLoc						= glGetUniformLocation(_program, "uAlbedo");
				_uMetalnessLoc					= glGetUniformLocation(_program, "uMetalness");
				_uRoughnessLoc					= glGetUniformLocation(_program, "uRoughness");
				_uHasAlbedoMapLoc				= glGetUniformLocation(_program, "uHasAlbedoMap");
				_uHasMetalnessRoughnessMapLoc	= glGetUniformLocation(_program, "uHasMetalnessRoughnessMap");
				_uHasNormalMapLoc				= glGetUniformLocation(_program, "uHasNormalMap");

				glCreateFramebuffers(1, &_fbo);
				_generateAndAttachMap(_fbo, &_positionMetalnessMap, 0);
				_generateAndAttachMap(_fbo, &_normalRoughnessMap, 1);
				_generateAndAttachMap(_fbo, &_albedoMap, 2);
				GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
				glNamedFramebufferDrawBuffers(_fbo, 3, DrawBuffers);

				glCreateRenderbuffers(1, &_rbo);
				glNamedRenderbufferStorage(_rbo, GL_DEPTH_COMPONENT, 1, 1);
				glNamedFramebufferRenderbuffer(_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);
			}
			~GeometryPassOGL() {
				glDeleteTextures(1, &_positionMetalnessMap);
				glDeleteTextures(1, &_normalRoughnessMap);
				glDeleteTextures(1, &_albedoMap);
			}

			GLuint getPositionMetalnessMap() { return _positionMetalnessMap; }
			GLuint getNormalRoughnessMap() { return _normalRoughnessMap; }
			GLuint getAlbedoMap() { return _albedoMap; }

			void resize(int p_width, int p_height) override {
				glBindTexture(GL_TEXTURE_2D, _positionMetalnessMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _normalRoughnessMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _ambientMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _albedoMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glNamedRenderbufferStorage(_rbo, GL_DEPTH_COMPONENT, p_width, p_height);
			}

			void execute(int p_viewport_width, int p_viewport_height, const std::map<Scene::Mesh*, Mesh*>& p_meshes_ogl, const std::map<Image*, Texture*>& p_textures_ogl) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_viewport_width, p_viewport_height);

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//glBindProgramPipeline(_program);
				glUseProgram(_program);

				for (Scene::Mesh* mesh : p_meshes) {
					for (unsigned int i = 0; i < mesh->getPrimitives().size();i++) {
						Scene::Primitive* primitive = mesh->getPrimitives()[i];

						glProgramUniform1i(_program, _uHasAlbedoMapLoc, primitive->getMaterial().getBaseColorMap() != nullptr);
						glProgramUniform3fv(_program, _uAlbedoLoc, 1, glm::value_ptr(primitive->getMaterial().getBaseColor()));
						if (primitive->getMaterial().getBaseColorMap() != nullptr) glBindTextureUnit(0, p_textures_ogl.at(primitive->getMaterial().getBaseColorMap())->getId());

						glProgramUniform1i(_program, _uHasMetalnessRoughnessMapLoc, primitive->getMaterial().getMetalnessRoughnessMap() != nullptr);
						glProgramUniform1f(_program, _uMetalnessLoc, primitive->getMaterial().getMetalness());
						glProgramUniform1f(_program, _uRoughnessLoc, primitive->getMaterial().getRoughness());
						if (primitive->getMaterial().getMetalnessRoughnessMap() != nullptr)  glBindTextureUnit(1, p_textures_ogl.at(primitive->getMaterial().getMetalnessRoughnessMap())->getId());

						glProgramUniform1i(_program, _uHasNormalMapLoc, primitive->getMaterial().getNormalMap() != nullptr);
						if (primitive->getMaterial().getNormalMap() != nullptr) glBindTextureUnit(2, p_textures_ogl.at(primitive->getMaterial().getNormalMap())->getId());

						p_meshes_ogl.at(mesh)->bind(i);
						glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)primitive->getIndices().size(), GL_UNSIGNED_INT, 0, (GLsizei)mesh->getSceneGraphNode().size());
						glBindVertexArray(0);
					}
				}
			}

	private:
		GLuint _fbo								= GL_INVALID_INDEX;
		GLuint _rbo								= GL_INVALID_INDEX;

		GLuint _positionMetalnessMap			= GL_INVALID_INDEX;
		GLuint _normalRoughnessMap				= GL_INVALID_INDEX;
		GLuint _albedoMap						= GL_INVALID_INDEX;

		GLint  _uAlbedoLoc						= GL_INVALID_INDEX;
		GLint  _uMetalnessLoc					= GL_INVALID_INDEX;
		GLint  _uRoughnessLoc					= GL_INVALID_INDEX;
		GLint  _uHasAlbedoMapLoc				= GL_INVALID_INDEX;
		GLint  _uHasMetalnessRoughnessMapLoc	= GL_INVALID_INDEX;
		GLint  _uHasNormalMapLoc				= GL_INVALID_INDEX;
	};
}
}
#endif