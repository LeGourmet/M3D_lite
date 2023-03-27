#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "primitive.hpp" 
#include "scene/objects/object.hpp"

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
        inline const std::vector<Primitive*>& getPrimitives() const { return _primitives; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addPrimitive(Primitive* p_primitive) { _primitives.push_back(p_primitive); }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        std::vector<Primitive*> _primitives;
    };
}
}

#endif