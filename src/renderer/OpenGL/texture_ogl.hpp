#ifndef __TEXTURE_OGL_HPP__
#define __TEXTURE_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/detail/_fixes.hpp"
#include "glm/gtc/integer.hpp"

#include "utils/define.hpp"
#include "scene/objects/meshes/texture.hpp"

namespace M3D
{
	namespace Renderer
	{
		class TextureOGL
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			TextureOGL(Scene::Texture* p_texture) {
				glCreateTextures(GL_TEXTURE_2D, 1, &_id);

				GLenum format, internalFormat;
				switch (p_texture->_image->getNbChannels()) {
					case 1: format = GL_RED; internalFormat = GL_R32F; break;
					case 2: format = GL_RG; internalFormat = GL_RG32F; break;
					case 3: format = GL_RGB; internalFormat = GL_RGB32F; break;
					default: format = GL_RGBA; internalFormat = GL_RGBA32F; break;
				}

				GLenum wrapping;
				switch (p_texture->_wrappingS) {
					case WRAPPING_TYPE::MIRRORED_REPEAT: wrapping = GL_MIRRORED_REPEAT; break;
					case WRAPPING_TYPE::CLAMP_TO_EDGE: wrapping = GL_CLAMP_TO_EDGE; break;
					default: wrapping = GL_REPEAT; break;
				}
				glTextureParameteri(_id, GL_TEXTURE_WRAP_S, wrapping);

				switch (p_texture->_wrappingT) {
					case WRAPPING_TYPE::MIRRORED_REPEAT: wrapping = GL_MIRRORED_REPEAT; break;
					case WRAPPING_TYPE::CLAMP_TO_EDGE: wrapping = GL_CLAMP_TO_EDGE; break;
					default: wrapping = GL_REPEAT; break;
				}
				glTextureParameteri(_id, GL_TEXTURE_WRAP_T, wrapping);

				if (p_texture->_minification == MINIFICATION_TYPE::MIN_LINEAR) { glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }
				else if (p_texture->_minification == MINIFICATION_TYPE::MIN_NEAREST) { glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST); }
				else {
					switch (p_texture->_minification) {
					case MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_NEAREST: glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
					case MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_LINEAR: glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
					case MINIFICATION_TYPE::MIN_LINEAR_MIPMAP_NEAREST: glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);break;
					default: glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
					}
					unsigned int lvMipMap = (unsigned int)glm::floor(glm::log2(glm::max<int>(p_texture->_image->getWidth(), p_texture->_image->getHeight()))) + 1;
					glTextureStorage2D(_id, lvMipMap, internalFormat, p_texture->_image->getWidth(), p_texture->_image->getHeight());
					glTextureSubImage2D(_id, 0, 0, 0, p_texture->_image->getWidth(), p_texture->_image->getHeight(), format, GL_UNSIGNED_BYTE, p_texture->_image->getData());
					glGenerateTextureMipmap(_id);
				}

				glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, (p_texture->_magnification == MAGNIFICATION_TYPE::MAG_LINEAR) ? GL_LINEAR : GL_NEAREST);
			}

			~TextureOGL() { glDeleteTextures(1, &_id); }

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getId() { return _id; }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _id = 0;
		};
	}
}

#endif
