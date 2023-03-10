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
        inline Material& getMaterial() const { return *_material; }
        inline std::vector<Vertex>& getVertices() { return _vertices; }
        inline std::vector<unsigned int>& getIndices() { return _indices; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addVertex(Vertex p_vertex) { _vertices.push_back(p_vertex); }

        /*void setIndices(std::vector<unsigned char> data) {
            _indices.reserve();
            memccpy(_indices.data(),data.data(), ,data.size());
        }*/

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Material* _material;
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
    };
}
}

#endif