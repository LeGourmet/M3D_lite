#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "primitive.hpp" 

#include <vector>

namespace M3D
{
namespace Scene
{
    class Mesh
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        Mesh() {}
        ~Mesh() { for(int i=0; i<_primitives.size() ;i++) delete _primitives[i]; }

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        std::vector<Primitive*> &getPrimitives() { return _primitives; }

        // --
        void addPrimitive(Primitive* p_primitive) { _primitives.push_back(p_primitive); }
        void addInstance(SceneGraphNode* p_node) { _instances.push_back(p_node); }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        std::vector<SceneGraphNode*> _instances;
        std::vector<Primitive*> _primitives;
    };
}
}

#endif