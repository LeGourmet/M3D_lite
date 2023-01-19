#ifndef __MESH_TRIANGLE_HPP__
#define __MESH_TRIANGLE_HPP__

#include "application.hpp"
#include "utils/define.hpp"
#include "renderer/texture.hpp"
#include "renderer/buffer.hpp"

namespace M3D
{
namespace Scene
{
    class MeshTriangle
    {
    public:
        MeshTriangle(const std::string &p_name);
        ~MeshTriangle();

        std::vector<Vertex> &getVertices() { return _vertices; }
        std::vector<unsigned int> &getIndices() { return _indices; }

        Renderer::Texture& getAmbientMap() const { return *_ambientMap; }
        Renderer::Texture& getDiffuseMap() const { return *_diffuseMap; }
        Renderer::Texture& getSpecularMap() const { return *_specularMap; }
        Renderer::Texture& getShininessMap() const { return *_shininessMap; }
        Renderer::Texture& getNormalMap() const { return *_normalMap; }
        Renderer::Buffer& getVAO() const { return *_vao; }

        void setAmbientMap(const std::string& p_path);
        void setDiffuseMap(const std::string& p_path);
        void setSpecularMap(const std::string& p_path);
        void setShininessMap(const std::string& p_path);
        void setNormalMap(const std::string& p_path);
        void setVAO();

        void addTriangle(const unsigned int p_v0, const unsigned int p_v1, const unsigned int p_v2);
        void addVertex(const Vertex p_vertex);

    public:
        std::string _name;

        Vec3f _ka = VEC3F_ZERO;
        Vec3f _kd = VEC3F_ZERO;
        Vec3f _ks = VEC3F_ZERO;
        float _s = 0.f;

        bool _hasUVs = false;
        bool _hasAmbientMap = false;
        bool _hasDiffuseMap = false;
        bool _hasSpecularMap = false;
        bool _hasShininessMap = false;
        bool _hasNormalMap = false;

        Mat4f _transformation = MAT4F_ID;
    private:
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;

        Renderer::Texture* _ambientMap = nullptr;
        Renderer::Texture* _diffuseMap = nullptr;
        Renderer::Texture* _specularMap = nullptr;
        Renderer::Texture* _shininessMap = nullptr;
        Renderer::Texture* _normalMap = nullptr;
        Renderer::Buffer*  _vao = nullptr;
    };
}
}

#endif
