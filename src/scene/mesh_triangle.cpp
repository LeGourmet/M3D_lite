#include "mesh_triangle.hpp"

#include "renderer/renderer_manager.hpp"
#include "application.hpp"

namespace M3D
{
    namespace Scene
    {
        MeshTriangle::MeshTriangle(const std::string& p_name)
            : _name(p_name) {
            _vertices = std::vector<Vertex>();
            _indices = std::vector<unsigned int>();
        };

        MeshTriangle::~MeshTriangle() {
            Application::getInstance().getRendererManager().getRenderer().deleteMesh(this);       
        }

        void MeshTriangle::addVertex(const Vertex p_vertex) { _vertices.push_back(p_vertex); }
        void MeshTriangle::addTriangle(const unsigned int p_v0, const unsigned int p_v1, const unsigned int p_v2) { 
            _indices.push_back(p_v0); 
            _indices.push_back(p_v1), 
            _indices.push_back(p_v2); 
        }
    }
}