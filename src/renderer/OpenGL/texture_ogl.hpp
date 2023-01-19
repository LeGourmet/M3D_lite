#ifndef __TEXTURE_OGL_HPP__
#define __TEXTURE_OGL_HPP__

#include "GL/gl3w.h"
#include "renderer/texture.hpp"
#include "utils/image.hpp"

#include <string>
#include <cmath>
#include <algorithm>

namespace M3D
{
    namespace Renderer
    {
        class TextureOGL : public Texture
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            TextureOGL(std::string p_path) {
				Image image;
				if (image.load(p_path)) {
					glCreateTextures(GL_TEXTURE_2D, 1, &_id);

					GLenum format, internalFormat;
					switch (image._nbChannels) {
						case 1: format = GL_RED; internalFormat = GL_R32F; break;
						case 2: format = GL_RG; internalFormat = GL_RG32F; break;
						case 3: format = GL_RGB; internalFormat = GL_RGB32F; break;
						default: format = GL_RGBA; internalFormat = GL_RGBA32F; break;
					}

					unsigned int lvMipMap = (unsigned int)std::floor(std::log2(std::max<int>(image._width, image._height))) + 1;
					glTextureStorage2D(_id, lvMipMap, internalFormat, image._width, image._height);
					glTextureParameteri(_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTextureParameteri(_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTextureSubImage2D(_id, 0, 0, 0, image._width, image._height, format, GL_UNSIGNED_BYTE, image._pixels);
					glGenerateTextureMipmap(_id);
				}
            }
            ~TextureOGL() { glDeleteTextures(1, &_id); }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void bind(int p_num) override { glBindTextureUnit(p_num, _id); }

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            GLuint _id = 0;
        };
    }
}
#endif