#ifndef __FINAL_PASS_OGL_HPP__
#define __FINAL_PASS_OGL_HPP__

#include "pass_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class FinalPassOGL : public PassOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			FinalPassOGL(std::string p_pathVert, std::string p_pathFrag) : PassOGL(p_pathVert, p_pathFrag) {
				_uGammaLoc = glGetUniformLocation(_program, "uGamma");

				glCreateVertexArrays(1, &_emptyVAO);
			}
			~FinalPassOGL() { glDeleteVertexArrays(1, &_emptyVAO); }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override { }

			void execute(float p_gamma, GLuint p_HDRMap) {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_program);

				glProgramUniform1f(_program, _uGammaLoc, p_gamma);
				glBindTextureUnit(0, p_HDRMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLint  _uGammaLoc = GL_INVALID_INDEX;

			GLuint _emptyVAO = GL_INVALID_INDEX;
		};
	}
}
#endif