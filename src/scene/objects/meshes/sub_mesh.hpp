#ifndef __SUB_MESH_HPP__
#define __SUB_MESH_HPP__

#include "vertex.hpp" 
#include "material.hpp"
#include "scene/acceleration_structures/AABB.hpp"

#include <vector>

namespace M3D
{
namespace Scene
{
    class SubMesh
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        SubMesh(Material* p_material) : _material(p_material) { _aabb = AABB(); }
        ~SubMesh() {}

        // ------------------------------------------------------ GETTERS ------------------------------------------------------
        inline const Material& getMaterial() const { return *_material; }
        inline const AABB& getAABB() const { return _aabb; }
        inline std::vector<Vertex>& getVertices() { return _vertices; }
        inline std::vector<unsigned int>& getIndices() { return _indices; }

        // ------------------------------------------------------ SETTERS ------------------------------------------------------
        void setIndices(const unsigned int* p_data, unsigned int p_count) {
            _indices.reserve(p_count);
            for (unsigned int i=0; i<p_count ;i++) _indices.push_back(p_data[i]);
        }

        void setIndices(const unsigned short* p_data, unsigned int p_count) {
            _indices.reserve(p_count);
            for(unsigned int i=0; i<p_count ;i++) _indices.push_back(p_data[i]);
        }

        void setIndices(const unsigned char* p_data, unsigned int p_count) {
            _indices.reserve(p_count);
            for(unsigned int i=0; i<p_count ;i++) _indices.push_back(p_data[i]);
        }

        // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
        void addVertex(Vertex p_vertex) { 
            _vertices.push_back(p_vertex); 
            _aabb.extend(p_vertex._position);
        }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Material*                   _material;
        AABB                        _aabb;
        std::vector<Vertex>         _vertices;
        std::vector<unsigned int>   _indices;
    };
}
}

#endif