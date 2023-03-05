#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "vertex.hpp" 

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
        ~Mesh() {}

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        //inline const Mat4f& getTransformation() { return _transformation; }

        inline std::vector<Vertex> &getVertices() { return _vertices; }
        inline std::vector<unsigned int> &getIndices() { return _indices; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        inline void addVertex(const Vertex p_vertex) { _vertices.push_back(p_vertex); }

        inline void addTriangle(const unsigned int p_v0, const unsigned int p_v1, const unsigned int p_v2) {
            _indices.push_back(p_v0); _indices.push_back(p_v1), _indices.push_back(p_v2);
        }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        //Mat4f _transformation = MAT4F_ID;
        
        std::vector<Vertex> _vertices = std::vector<Vertex>();
        std::vector<unsigned int> _indices = std::vector<unsigned int>();
    };
}
}

#endif
