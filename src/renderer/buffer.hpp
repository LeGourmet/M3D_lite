#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

namespace M3D
{
    namespace Renderer
    {
        class Buffer
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Buffer() {}
            virtual ~Buffer() = default;
            
            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            virtual void bind() = 0;
        };
    }
}
#endif
