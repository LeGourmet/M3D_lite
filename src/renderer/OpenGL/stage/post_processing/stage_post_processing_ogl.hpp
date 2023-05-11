#ifndef __PASS_POST_PROCESSING_OGL_HPP__
#define __PASS_POST_PROCESSING_OGL_HPP__

#include "GL/gl3w.h"

#include "../stage_ogl.hpp"

#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class PassPostProcessingOGL : public StageOGL{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			PassPostProcessingOGL() { 
				_toneMappingPass.addUniform("uGama");
			}

			~PassPostProcessingOGL() { }

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getShadingMap() { return _shadingMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) { }

			void execute(float p_gamma, GLuint p_HDRMap) {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_toneMappingPass.getProgram());

				glProgramUniform1f(_toneMappingPass.getProgram(), _toneMappingPass.getUniform("uGamma"), p_gamma);
				glBindTextureUnit(0, p_HDRMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _shadingMap	= GL_INVALID_INDEX;

			ProgramOGL _toneMappingPass = ProgramOGL("src/renderer/shaders/utils/quadScreen.vert", "", "src/renderer/shaders/post_processing/toneMapping.frag");
		};
	}
}
#endif