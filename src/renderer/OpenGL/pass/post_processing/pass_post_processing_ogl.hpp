#ifndef __PASS_POST_PROCESSING_OGL_HPP__
#define __PASS_POST_PROCESSING_OGL_HPP__

#include "GL/gl3w.h"

#include "program_tone_mapping_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class PassPostProcessingOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			PassPostProcessingOGL() {
				glCreateFramebuffers(1, &_fbo);
				
				glCreateTextures(GL_TEXTURE_2D, 1, &_shadingMap);
				glTextureParameteri(_shadingMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(_shadingMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _shadingMap, 0);

				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glNamedFramebufferDrawBuffers(_fbo, 1, DrawBuffers);
			}

			~PassPostProcessingOGL() { }

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			GLuint getShadingMap() { return _shadingMap; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) { 
				_programToneMapping.resize(p_width, p_height);
				
				glBindTexture(GL_TEXTURE_2D, _shadingMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_width, p_height);
			}

			void execute(float p_gamma, GLuint p_deferredShadingMap, GLuint p_forwardShadingMap) {
				_programToneMapping.execute(p_gamma, p_deferredShadingMap, p_forwardShadingMap, 0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fbo			= GL_INVALID_INDEX;
			GLuint _shadingMap	= GL_INVALID_INDEX;

			ProgramToneMappingOGL _programToneMapping = ProgramToneMappingOGL();
		};
	}
}
#endif