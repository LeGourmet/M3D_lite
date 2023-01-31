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
        Vec3f _ambient = VEC3F_ZERO;
        Vec3f _albedo = VEC3F_ZERO;
        float _metalness = 0.8f;
        float _roughness = 1.f;

        bool _hasUVs = false;
        bool _hasAmbientMap = false;
        bool _hasAlbedoMap = false;
        bool _hasMetalnessMap = false;
        bool _hasRoughnessMap = false;
        bool _hasNormalMap = false;

        std::string _ambientMapPath;
        std::string _albedoMapPath;
        std::string _metalnessMapPath;
        std::string _roughnessMapPath;
        std::string _normalMapPath;

        Mat4f _transformation = MAT4F_ID;

    private:
        std::vector<Vertex> _vertices = std::vector<Vertex>();
        std::vector<unsigned int> _indices = std::vector<unsigned int>();
    };
}
}

#endif
