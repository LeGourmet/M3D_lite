#ifndef __STAGE_MESH_TRANSPARENT_OGL_HPP__
#define __STAGE_MESH_TRANSPARENT_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"

#include "stage_ogl.hpp"

#include "application.hpp"
#include "renderer/renderer.hpp"
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
				_transparencyPass.addUniform("uNbFragmentsMaxPerPixel");

				glCreateFramebuffers(1, &_fbo);
				generateMap(&_transparencyMap, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fbo, _transparencyMap, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fbo, 1, DrawBuffers);

				glCreateVertexArrays(1, &_emptyVAO);
			}

			~StageMeshTransparentOGL() { 
				glDeleteTextures(1, &_transparencyMap);
				glDeleteFramebuffers(1, &_fbo);
				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getTransparencyMap() { return _transparencyMap; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _transparencyMap);
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_opaqueMap, GLuint p_depthMapOpaque, GLuint p_rootTransparency, GLuint p_ssboTransparency) {
				glViewport(0, 0, p_width, p_height);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

				glClear(GL_COLOR_BUFFER_BIT);

				glUseProgram(_transparencyPass.getProgram());

				glBindTextureUnit(0, p_opaqueMap);
				glBindTextureUnit(1, p_depthMapOpaque);

				glBindImageTexture(2, p_rootTransparency, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, p_ssboTransparency);

				glProgramUniform1ui(_transparencyPass.getProgram(), _transparencyPass.getUniform("uNbFragmentsMaxPerPixel"), Application::getInstance().getRenderer().getTransparencyMaxDepth());
			
				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo				= GL_INVALID_INDEX;

			GLuint _emptyVAO		= GL_INVALID_INDEX;

			GLuint _transparencyMap = GL_INVALID_INDEX;

			ProgramOGL _transparencyPass = ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/geometry/transparentPass.frag");
		};
	}
}

#endif
