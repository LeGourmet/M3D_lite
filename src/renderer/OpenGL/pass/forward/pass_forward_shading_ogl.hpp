#ifndef __PASS_FORWARD_SHADING_OGL_HPP__
#define __PASS_FORWARD_SHADING_OGL_HPP__

#include "GL/gl3w.h"

namespace M3D
{
	namespace Renderer
	{
		class PassForwardShadingOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			PassForwardShadingOGL() {
				glCreateFramebuffers(1, &_fbo);
				
				glCreateTextures(GL_TEXTURE_2D, 1, &_shadingMap);
				glTextureParameteri(_shadingMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(_shadingMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _shadingMap, 0);

				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fbo, 1, DrawBuffers);
			}

			~PassForwardShadingOGL() { }

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getShadingMap() { return _shadingMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) {
				glBindTexture(GL_TEXTURE_2D, _shadingMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_width, p_height);
			}

			void execute(float p_gamma, GLuint p_HDRMap) {

			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo = GL_INVALID_INDEX;

			GLuint _shadingMap = GL_INVALID_INDEX;
		};
	}
}
#endif