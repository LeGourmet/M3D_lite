#ifndef __STAGE_OGL_HPP__
#define __STAGE_OGL_HPP__

#include "GL/gl3w.h"

#include <map>
#include <string>

namespace M3D
{
	namespace Renderer
	{
		class StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageOGL() {}

			~StageOGL() {
				glDeleteTextures(_attachements.size(), &_attachements.data());
				glDeleteRenderbuffers(1, &_RBOs.at());
				glDeleteFramebuffers(1, &_fbo);
				if (!(_rbo == 0 || _rbo == GL_INVALID_INDEX)) glDeleteRenderbuffers(1, &_rbo);
				if (!(_fbo == 0 || _fbo == GL_INVALID_INDEX)) glDeleteFramebuffers(1, &_fbo);
			}

			void resizeFbo(int p_width, int p_height) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
				glViewport(0, 0, p_width, p_height);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			void resizeRbo(int p_width, int p_height) {
				glNamedRenderbufferStorage(_rbo, GL_DEPTH_COMPONENT, p_width, p_height);
			}

			void generateMap(GLuint* p_texture) {
				glCreateTextures(GL_TEXTURE_2D, 1, p_texture);
				glTextureParameteri(*p_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			void generateCubeMap(GLuint* p_texture) {
				glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, p_texture);
				glTextureParameteri(*p_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			}

			void generateAndAttachColorMap(GLuint p_framebuffer, GLuint* p_texture, int p_id) {
				generateMap(p_texture);
				glNamedFramebufferTexture(p_framebuffer, GL_COLOR_ATTACHMENT0 + p_id, *p_texture, 0);
			}

			void generateAndAttachDepthMap(GLuint p_framebuffer, GLuint* p_texture) {
				generateMap(p_texture);
				glNamedFramebufferTexture(p_framebuffer, GL_DEPTH_ATTACHMENT, *p_texture, 0);
			}

			void resizeColorMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_2D, p_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			void resizeColorCubeMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, p_texture);
				for (unsigned int i = 0; i < 6; i++)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}

			void resizeDepthMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_2D, p_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			void resizeDepthCubeMap(int p_width, int p_height, GLuint p_texture) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, p_texture);
				for (unsigned int i = 0; i < 6; i++)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}

		protected:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			std::map<std::string,GLuint> _FBOs;
			std::map<std::string,GLuint> _RBOs;
			std::map<std::string,GLuint> _attachements; 
		};
	}
}
#endif