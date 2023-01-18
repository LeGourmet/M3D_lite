#include "mesh_triangle.hpp"

#include "application.hpp"

namespace M3D
{
    namespace Scene
    {
        void MeshTriangle::setAmbientMap(const std::string& p_path) { _ambientMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setDiffuseMap(const std::string& p_path) { _diffuseMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setSpecularMap(const std::string& p_path) { _specularMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setShininessMap(const std::string& p_path) { _shininessMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setNormalMap(const std::string& p_path) { _normalMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setRendererDatas() { _rendererDatas = Application::getInstance().getRendererManager().getRenderer().createObject(_vertices, _indices); }

        void MeshTriangle::addVertex(const Vertex p_vertex) { _vertices.push_back(p_vertex); }
        void MeshTriangle::addTriangle(const unsigned int p_v0, const unsigned int p_v1, const unsigned int p_v2) { 
            _indices.push_back(p_v0); 
            _indices.push_back(p_v1), 
            _indices.push_back(p_v2); 
        }
    }
}