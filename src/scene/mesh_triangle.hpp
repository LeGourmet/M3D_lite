#ifndef __MESH_TRIANGLE_HPP__
#define __MESH_TRIANGLE_HPP__

#include "application.hpp"
#include "utils/define.hpp"

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

        unsigned int getIdAmbientMap() const { return _idAmbientMap; }
        unsigned int getIdDiffuseMap() const { return _idDiffuseMap; }
        unsigned int getIdSpecularMap() const { return _idSpecularMap; }
        unsigned int getIdShininessMap() const { return _idShininessMap; }
        unsigned int getIdNormalMap() const { return _idNormalMap; }
        unsigned int getIdVAO() const { return _idVao; }

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
        unsigned int _idAmbientMap = 0;
        unsigned int _idDiffuseMap = 0;
        unsigned int _idSpecularMap = 0;
        unsigned int _idShininessMap = 0;
        unsigned int _idNormalMap = 0;
        unsigned int _idVao = 0;

        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
    };
}
}

#endif
