#ifndef __STAGE_POST_PROCESSING_OGL_HPP__
#define __STAGE_POST_PROCESSING_OGL_HPP__

#include "GL/gl3w.h"

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
		class StagePostProcessingOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StagePostProcessingOGL() { 
				_toneMappingPass.addUniform("uGamma");

				glCreateVertexArrays(1, &_emptyVAO);
			}

			~StagePostProcessingOGL() { 
				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getShadingMap() { return _shadingMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) { }

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Image*, TextureOGL*> p_textures, GLuint p_HDRMap) {
				glViewport(0, 0, p_width, p_height);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_toneMappingPass.getProgram());

				glProgramUniform1f(_toneMappingPass.getProgram(), _toneMappingPass.getUniform("uGamma"), Application::getInstance().getRenderer().getGamma());
				glBindTextureUnit(0, p_HDRMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _shadingMap	= GL_INVALID_INDEX;

			GLuint _emptyVAO	= GL_INVALID_INDEX;

			ProgramOGL _toneMappingPass = ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/toneMappingPass.frag");
		};
	}
}
#endif