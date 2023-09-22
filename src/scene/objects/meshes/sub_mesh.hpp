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
        SubMesh(Material* p_material, std::vector<Vertex>& p_vertices, const unsigned int* p_data, unsigned int p_count) {
            _material = p_material;
            _vertices = p_vertices;
            _indices.reserve(p_count);
            for (unsigned int i = 0; i < p_count;i++) _indices.push_back(p_data[i]);
        }

        SubMesh(Material* p_material, std::vector<Vertex>& p_vertices, const unsigned short* p_data, unsigned int p_count) {
            _material = p_material;
            _vertices = p_vertices;
            _indices.reserve(p_count);
            for (unsigned int i = 0; i < p_count;i++) _indices.push_back(p_data[i]);
        }
        
        SubMesh(Material* p_material, std::vector<Vertex>& p_vertices, const unsigned char* p_data, unsigned int p_count) {
            _material = p_material;
            _vertices = p_vertices;
            _indices.reserve(p_count);
            for (unsigned int i = 0; i < p_count;i++) _indices.push_back(p_data[i]);
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