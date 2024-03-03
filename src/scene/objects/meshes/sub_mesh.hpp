#ifndef __SUB_MESH_HPP__
#define __SUB_MESH_HPP__

#include "vertex.hpp" 
#include "material.hpp"

#include <vector>

namespace M3D
{
namespace Scene
{
    class SubMesh
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        SubMesh(Material* p_material, std::vector<Vertex>& p_vertices, std::vector<unsigned int>& p_indices) {
            _material = p_material;
            _vertices = p_vertices;
            _indices = p_indices;
        }

        ~SubMesh() {}

        // ------------------------------------------------------ GETTERS ------------------------------------------------------
        inline const Material& getMaterial() const { return *_material; }
        inline std::vector<Vertex>& getVertices() { return _vertices; }
        inline std::vector<unsigned int>& getIndices() { return _indices; }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Material*                   _material;
        std::vector<Vertex>         _vertices;
        std::vector<unsigned int>   _indices;
    };
}
}

#endif