#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "primitive.hpp" 
#include "../object.hpp"

#include <vector>

namespace M3D
{
namespace Scene
{
    class Mesh : public Object
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        Mesh() {}
        ~Mesh() { for(int i=0; i<_primitives.size() ;i++) delete _primitives[i]; }

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        inline std::vector<Primitive*>& getPrimitives() { return _primitives; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addPrimitive(Primitive* p_primitive) { _primitives.push_back(p_primitive); }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        std::vector<Primitive*> _primitives;
    };
}
}

#endif