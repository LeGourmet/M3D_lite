#include "mesh_triangle.hpp"

#include "application.hpp"
#include "renderer/renderer_manager.hpp"

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
            Application::getInstance().getRendererManager().getRenderer().deleteTexture(_idAmbientMap);
            Application::getInstance().getRendererManager().getRenderer().deleteTexture(_idDiffuseMap);
            Application::getInstance().getRendererManager().getRenderer().deleteTexture(_idSpecularMap);
            Application::getInstance().getRendererManager().getRenderer().deleteTexture(_idShininessMap);
            Application::getInstance().getRendererManager().getRenderer().deleteTexture(_idNormalMap);
            Application::getInstance().getRendererManager().getRenderer().deleteVAO(_idVao);           
        }

        void MeshTriangle::setAmbientMap(const std::string& p_path) { _idAmbientMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setDiffuseMap(const std::string& p_path) { _idDiffuseMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setSpecularMap(const std::string& p_path) { _idSpecularMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setShininessMap(const std::string& p_path) { _idShininessMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setNormalMap(const std::string& p_path) { _idNormalMap = Application::getInstance().getRendererManager().getRenderer().createTexture(p_path); }
        void MeshTriangle::setVAO() { _idVao = Application::getInstance().getRendererManager().getRenderer().createVAO(_vertices, _indices); }

        void MeshTriangle::addVertex(const Vertex p_vertex) { _vertices.push_back(p_vertex); }
        void MeshTriangle::addTriangle(const unsigned int p_v0, const unsigned int p_v1, const unsigned int p_v2) { 
            _indices.push_back(p_v0); 
            _indices.push_back(p_v1), 
            _indices.push_back(p_v2); 
        }
    }
}