#ifndef __TEXTURE_OGL_HPP__
#define __TEXTURE_OGL_HPP__

#include "../texture.hpp"

namespace M3D
{
    namespace Renderer
    {
        class TextureOGL : public Texture
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            TextureOGL() {}
            ~TextureOGL() {}

            void bind(int p_num) override { glBindTextureUnit(p_num, _id); }
        private:
            GLuint _id;
        };
    }
}