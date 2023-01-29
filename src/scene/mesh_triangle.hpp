#ifndef __MESH_TRIANGLE_HPP__
#define __MESH_TRIANGLE_HPP__

#include "utils/define.hpp"

namespace M3D
{
namespace Scene
{
    class MeshTriangle
    {
    public:
        MeshTriangle();
        ~MeshTriangle();

        std::vector<Vertex> &getVertices() { return _vertices; }
        std::vector<unsigned int> &getIndices() { return _indices; }

        void addTriangle(const unsigned int p_v0, const unsigned int p_v1, const unsigned int p_v2);
        void addVertex(const Vertex p_vertex);

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

        std::string _ambientMapPath;
        std::string _diffuseMapPath;
        std::string _specularMapPath;
        std::string _shininessMapPath;
        std::string _normalMapPath;

        Mat4f _transformation = MAT4F_ID;

    private:
        std::vector<Vertex> _vertices = std::vector<Vertex>();
        std::vector<unsigned int> _indices = std::vector<unsigned int>();
    };
}
}

#endif
