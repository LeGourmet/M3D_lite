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
				_uMatrix_MVPLoc			= glGetUniformLocation(_program, "uMatrix_MVP");
				_uMatrix_MLoc			= glGetUniformLocation(_program, "uMatrix_M");
				_uMatrix_NormalLoc		= glGetUniformLocation(_program, "uMatrix_Normal");
				_uAmbientLoc			= glGetUniformLocation(_program, "uAmbient");
				_uAlbedoLoc				= glGetUniformLocation(_program, "uAlbedo");
				_uMetalnessLoc			= glGetUniformLocation(_program, "uMetalness");
				_uRoughnessLoc			= glGetUniformLocation(_program, "uRoughness");
				_uHasAmbientMapLoc		= glGetUniformLocation(_program, "uHasAmbientMap");
				_uHasAlbedoMapLoc		= glGetUniformLocation(_program, "uHasAlbedoMap");
				_uHasMetalnessMapLoc	= glGetUniformLocation(_program, "uHasMetalnessMap");
				_uHasRoughnessMapLoc	= glGetUniformLocation(_program, "uHasRoughnessMap");
				_uHasNormalMapLoc		= glGetUniformLocation(_program, "uHasNormalMap");

				glGenFramebuffers(1, &_fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				_generateAndAttachMap(&_positionMetalnessMap, 0);
				_generateAndAttachMap(&_normalRoughnessMap, 1);
				_generateAndAttachMap(&_ambientMap, 2);
				_generateAndAttachMap(&_albedoMap, 3);
				GLenum DrawBuffers0[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(4, DrawBuffers0);

				glGenRenderbuffers(1, &_rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1, 1);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);	
			}
			~GeometryPassOGL() {
				glDeleteTextures(1, &_ambientMap);
				glDeleteTextures(1, &_positionMetalnessMap);
				glDeleteTextures(1, &_normalRoughnessMap);
				glDeleteTextures(1, &_albedoMap);
			}

			GLuint getAmbientMap()			 { return _ambientMap; }
			GLuint getPositionMetalnessMap() { return _positionMetalnessMap; }
			GLuint getNormalRoughnessMap()	 { return _normalRoughnessMap; }
			GLuint getAlbedoMap()			 { return _albedoMap; }

			void resize(int p_width, int p_height) override {
				glBindTexture(GL_TEXTURE_2D, _positionMetalnessMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _normalRoughnessMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _ambientMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, _albedoMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, p_width, p_height);
			}

			// pointer ==> vraiment mieux
			void execute(int p_viewport_width, int p_viewport_height, std::vector<Scene::MeshTriangle*> p_meshes, std::map<Scene::MeshTriangle*, MeshOGL*> p_meshes_ogl) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_viewport_width, p_viewport_height);

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_program);

				const Mat4f VP = Application::getInstance().getSceneManager().getCamera().getProjectionMatrix() * Application::getInstance().getSceneManager().getCamera().getViewMatrix();

				for (Scene::MeshTriangle* mesh : p_meshes) {
					glProgramUniformMatrix4fv(_program, _uMatrix_MVPLoc, 1, false, glm::value_ptr(VP * mesh->_transformation));
					glProgramUniformMatrix4fv(_program, _uMatrix_MLoc, 1, false, glm::value_ptr(mesh->_transformation));
					glProgramUniformMatrix4fv(_program, _uMatrix_NormalLoc, 1, false, glm::value_ptr(glm::transpose(mesh->_transformation)));
					MeshOGL* meshOGL = p_meshes_ogl.at(mesh);

					glProgramUniform1i(_program, _uHasAmbientMapLoc, mesh->_hasAmbientMap);
					glProgramUniform3fv(_program, _uAmbientLoc, 1, glm::value_ptr(mesh->_ambient));
					if (mesh->_hasAmbientMap) glBindTextureUnit(0, meshOGL->getIdAmbientMap());

					glProgramUniform1i(_program, _uHasAlbedoMapLoc, mesh->_hasAlbedoMap);
					glProgramUniform3fv(_program, _uAlbedoLoc, 1, glm::value_ptr(mesh->_albedo));
					if (mesh->_hasAlbedoMap) glBindTextureUnit(1, meshOGL->getIdAlbedoMap());

					glProgramUniform1i(_program, _uHasMetalnessMapLoc, mesh->_hasMetalnessMap);
					glProgramUniform1f(_program, _uMetalnessLoc, mesh->_metalness);
					if (mesh->_hasMetalnessMap)  glBindTextureUnit(2, meshOGL->getIdMetalnessMap());

					glProgramUniform1i(_program, _uHasRoughnessMapLoc, mesh->_hasRoughnessMap);
					glProgramUniform1f(_program, _uRoughnessLoc, mesh->_roughness);
					if (mesh->_hasRoughnessMap) glBindTextureUnit(3, meshOGL->getIdRoughnessMap());

					glProgramUniform1i(_program, _uHasNormalMapLoc, mesh->_hasNormalMap);
					if (mesh->_hasNormalMap) glBindTextureUnit(4, meshOGL->getIdNormalMap());

					glBindVertexArray(meshOGL->getVao());
					glDrawElements(GL_TRIANGLES, (GLsizei)mesh->getIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}

	private:
		GLuint _fbo						= GL_INVALID_INDEX;
		GLuint _rbo						= GL_INVALID_INDEX;
		GLuint _ambientMap				= GL_INVALID_INDEX;
		GLuint _positionMetalnessMap	= GL_INVALID_INDEX;
		GLuint _normalRoughnessMap		= GL_INVALID_INDEX;
		GLuint _albedoMap				= GL_INVALID_INDEX;

		GLint  _uMatrix_MVPLoc			= GL_INVALID_INDEX;
		GLint  _uMatrix_MLoc			= GL_INVALID_INDEX;
		GLint  _uMatrix_NormalLoc		= GL_INVALID_INDEX;
		GLint  _uAmbientLoc				= GL_INVALID_INDEX;
		GLint  _uAlbedoLoc				= GL_INVALID_INDEX;
		GLint  _uMetalnessLoc			= GL_INVALID_INDEX;
		GLint  _uRoughnessLoc			= GL_INVALID_INDEX;
		GLint  _uHasAmbientMapLoc		= GL_INVALID_INDEX;
		GLint  _uHasAlbedoMapLoc		= GL_INVALID_INDEX;
		GLint  _uHasMetalnessMapLoc		= GL_INVALID_INDEX;
		GLint  _uHasRoughnessMapLoc		= GL_INVALID_INDEX;
		GLint  _uHasNormalMapLoc		= GL_INVALID_INDEX;
	};
}
}
#endif