#ifndef __TEXTURE_OGL_HPP__
#define __TEXTURE_OGL_HPP__

#include "utils/define.hpp"
#include "utils/image.hpp"

#include "GL/gl3w.h"
#include "glm/gtc/integer.hpp"

namespace M3D
{
	namespace Renderer
	{
		class TextureOGL
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			TextureOGL(Image* p_texture) {
				glCreateTextures(GL_TEXTURE_2D, 1, &_id);

				glTextureParameteri(_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				
				// TODO respect internal format, bit depth ...
				GLenum format, internalFormat;
				switch ( p_texture->getNbChannels()) {
					case 1: format = GL_RED; internalFormat = GL_R32F; break;
					case 2: format = GL_RG; internalFormat = GL_RG32F; break;
					case 3: format = GL_RGB; internalFormat = GL_RGB32F; break;
					default: format = GL_RGBA; internalFormat = GL_RGBA32F; break;
				}

				unsigned int lvMipMap = (unsigned int)glm::floor(glm::log2(glm::max<int>(p_texture->getWidth(), p_texture->getHeight()))) + 1;
				glTextureStorage2D(_id, lvMipMap, internalFormat, p_texture->getWidth(), p_texture->getHeight());
				glTextureSubImage2D(_id, 0, 0, 0, p_texture->getWidth(), p_texture->getHeight(), format, GL_UNSIGNED_BYTE, p_texture->getData());
				glGenerateTextureMipmap(_id);
			}

			~TextureOGL() { glDeleteTextures(1, &_id); }

			// ------------------------------------------------------ GETTERS --------------------------------------------------------
			GLuint getId() { return _id; }

		private:
			// ----------------------------------------------------- ATTRIBUTS -------------------------------------------------------
			GLuint _id = 0;
		};
	}
}
#endif