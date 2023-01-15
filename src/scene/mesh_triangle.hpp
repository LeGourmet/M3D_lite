#ifndef __MESH_TRIANGLE_HPP__
#define __MESH_TRIANGLE_HPP__

#include "application.hpp"
#include "utils/define.hpp"
#include "renderer/texture.hpp"

namespace M3D
{
namespace Scene
{
    class MeshTriangle
    {
    public:
        MeshTriangle(const std::string &p_name)
            : _name(p_name), _ambientMap(), _diffuseMap(), _specularMap(), _normalMap(), _shininessMap() {};

        ~MeshTriangle() = default;

        std::string getName() { return _name; }

        size_t getNbTriangles() const { return _triangles.size(); }
        size_t getNbVertices() const { return _vertices.size(); }

        std::vector<Vec3f> &getVertices() { return _vertices; }
        std::vector<Vec3f> &getNormals() { return _normals; }
        std::vector<Vec3u> &getTriangles() { return _triangles; }
        std::vector<Vec2f> &getUvs() { return _uvs; }
        std::vector<Vec3f> &getTangents() { return _tangents; }
        std::vector<Vec3f> &getBitangents() { return _bitangents; }

        Renderer::Texture const * getAmbientMap() { return &_ambientMap; }
        Renderer::Texture const * getDiffuseMap() { return &_diffuseMap; }
        Renderer::Texture const * getSpecularMap() { return &_specularMap; }
        Renderer::Texture const * getShininessMap() { return &_shininessMap; }
        Renderer::Texture const * getNormalMap() { return &_normalMap; }

        void setAmbientMap(const std::string &p_path) { _ambientMap = Application::getInstance().getRenderer().createTexture(p_path); }
        void setDiffuseMap(const std::string &p_path) { _diffuseMap = Application::getInstance().getRenderer().createTexture(p_path); }
        void setSpecularMap(const std::string &p_path) { _specularMap = Application::getInstance().getRenderer().createTexture(p_path); }
        void setShininessMap(const std::string &p_path) { _shininessMap = Application::getInstance().getRenderer().createTexture(p_path); }
        void setNormalMap(const std::string &p_path) { _normalMap = Application::getInstance().getRenderer().createTexture(p_path); }

        void addTriangle(const unsigned int p_v0, const unsigned int p_v1, const unsigned int p_v2) { _triangles.emplace_back(p_v0, p_v1, p_v2); }
        void addVertex(const float p_x, const float p_y, const float p_z) { _vertices.emplace_back(p_x, p_y, p_z); }
        void addNormal(const float p_x, const float p_y, const float p_z) { _normals.emplace_back(p_x, p_y, p_z); }
        void addUV(const float p_u, const float p_v) { _uvs.emplace_back(p_u, p_v); }
        void addTangent(const float p_x, const float p_y, const float p_z) { _tangents.emplace_back(p_x, p_y, p_z); }
        void addBitangent(const float p_x, const float p_y, const float p_z) { _bitangents.emplace_back(p_x, p_y, p_z); }

    public:
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
        std::string _name;

        std::vector<Vec3f> _vertices;
        std::vector<Vec3f> _normals;
        std::vector<Vec3u> _triangles;
        std::vector<Vec2f> _uvs;
        std::vector<Vec3f> _tangents;
        std::vector<Vec3f> _bitangents;

        Renderer::Texture _ambientMap;
        Renderer::Texture _diffuseMap;
        Renderer::Texture _specularMap;
        Renderer::Texture _shininessMap;
        Renderer::Texture _normalMap;
    };
}
}

#endif
