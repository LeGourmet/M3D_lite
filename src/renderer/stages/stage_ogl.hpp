#pragma once

#include "GL/gl3w.h"

namespace M3D
{
	class StageOGL {
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		StageOGL() {}
		~StageOGL() {}

		void generateMap(GLuint* p_texture, GLenum p_min, GLenum p_mag, GLenum p_wrapS, GLenum p_wrapT) {
			glCreateTextures(GL_TEXTURE_2D, 1, p_texture);
			glTextureParameteri(*p_texture, GL_TEXTURE_MIN_FILTER, p_min);
			glTextureParameteri(*p_texture, GL_TEXTURE_MAG_FILTER, p_mag);
			glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_S, p_wrapS);
			glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_T, p_wrapT);
		}

		void generateCubeMap(GLuint* p_texture, GLenum p_min, GLenum p_mag, GLenum p_wrapS, GLenum p_wrapT, GLenum p_wrapR) {
			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, p_texture);
			glTextureParameteri(*p_texture, GL_TEXTURE_MAG_FILTER, p_min);
			glTextureParameteri(*p_texture, GL_TEXTURE_MIN_FILTER, p_mag);
			glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_S, p_wrapS);
			glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_T, p_wrapT);
			glTextureParameteri(*p_texture, GL_TEXTURE_WRAP_R, p_wrapR);
		}

		void attachColorMap(GLuint p_framebuffer, GLuint p_texture, uint p_id) {
			glNamedFramebufferTexture(p_framebuffer, GL_COLOR_ATTACHMENT0 + p_id, p_texture, 0);
		}

		void attachDepthMap(GLuint p_framebuffer, GLuint p_texture) {
			glNamedFramebufferTexture(p_framebuffer, GL_DEPTH_ATTACHMENT, p_texture, 0);
		}

		void resizeRbo(uint p_width, uint p_height, GLuint p_rbo) {
			glNamedRenderbufferStorage(p_rbo, GL_DEPTH_COMPONENT, p_width, p_height);
		}

		void resizeColorMap(uint p_textureFormat, uint p_pixelFormat, uint p_pixelType, uint p_width, uint p_height, GLuint p_texture) {
			glBindTexture(GL_TEXTURE_2D, p_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, p_textureFormat, p_width, p_height, 0, p_pixelFormat, p_pixelType, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void resizeColorCubeMap(uint p_textureFormat, uint p_pixelFormat, uint p_pixelType, uint p_width, uint p_height, GLuint p_texture) {
			glBindTexture(GL_TEXTURE_CUBE_MAP, p_texture);
			for (uint i = 0; i < 6; i++)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, p_textureFormat, p_width, p_height, 0, p_pixelFormat, p_pixelType, 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		void resizeDepthMap(uint p_width, uint p_height, GLuint p_texture) {
			glBindTexture(GL_TEXTURE_2D, p_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void resizeDepthCubeMap(uint p_width, uint p_height, GLuint p_texture) {
			glBindTexture(GL_TEXTURE_CUBE_MAP, p_texture);
			for (uint i = 0; i < 6; i++)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
	};
}
