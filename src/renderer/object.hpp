#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

namespace M3D
{
    namespace Renderer
    {
        class Object
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Object() {}
            virtual ~Object() = default;
            
            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            virtual void bind() = 0;
        };
    }
}
#endif
