#ifndef __PRIMITIVE_HPP__
#define __PRIMITIVE_HPP__

#include "vertex.hpp" 
#include "material.hpp"

#include <vector>

namespace M3D
{
namespace Scene
{
    class Primitive
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        Primitive(Material* p_material): _material(p_material) {}
        ~Primitive() {}

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        inline const Material& getMaterial() const { return *_material; }
        inline std::vector<Vertex>& getVertices() { return _vertices; }
        inline std::vector<unsigned int>& getIndices() { return _indices; }

        // ----------------------------------------------------- SETTERS -------------------------------------------------------
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

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addVertex(const Vertex& p_vertex) { _vertices.push_back(p_vertex); }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Material* _material;
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
    };
}
}

#endif