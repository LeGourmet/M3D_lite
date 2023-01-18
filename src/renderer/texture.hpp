#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

namespace M3D
{
    namespace Renderer
    {
        class Texture
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Texture() {}
            virtual ~Texture() = default;

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            virtual void bind(int p_id) = 0;
        };
    }
}
#endif
