#ifndef __PROGRAM_TONE_MAPPING_OGL_HPP__
#define __PROGRAM_TONE_MAPPING_OGL_HPP__

#include "../program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class ProgramToneMappingOGL : public ProgramOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ProgramToneMappingOGL() : ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert","","src/renderer/OpenGL/shaders/toneMappingPass.frag") {
				_uGammaLoc = glGetUniformLocation(_program, "uGamma");

				glCreateVertexArrays(1, &_emptyVAO);
			}
			~ProgramToneMappingOGL() { glDeleteVertexArrays(1, &_emptyVAO); }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override { }

			void execute(float p_gamma, GLuint p_deferredShadingMap, GLuint p_forwardShadingMap, GLuint p_fbo) {
				glBindFramebuffer(GL_FRAMEBUFFER, p_fbo);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_program);

				glProgramUniform1f(_program, _uGammaLoc, p_gamma);
				glBindTextureUnit(0, p_deferredShadingMap);
				glBindTextureUnit(1, p_forwardShadingMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _uGammaLoc = GL_INVALID_INDEX;

			GLuint _emptyVAO = GL_INVALID_INDEX;
		};
	}
}
#endif