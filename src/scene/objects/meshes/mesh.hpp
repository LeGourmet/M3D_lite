#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "sub_mesh.hpp" 
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
            Mesh() { }
            ~Mesh() { }

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            inline std::vector<SubMesh>& getSubMeshes() { return _subMeshes; }

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void addSubMesh(SubMesh p_subMeshes) { _subMeshes.push_back(p_subMeshes); }

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            std::vector<SubMesh> _subMeshes;
        };
    }
}

#endif
