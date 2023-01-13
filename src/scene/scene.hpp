#ifndef __SCENE__
#define __SCENE__

#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "engine/engine.hpp"
#include "mesh_triangle.hpp"

#include <string>
#include <vector>

namespace M3D
{
namespace Scene
{
    class Scene
    {
    public:
        Scene() {}
        ~Scene() {}

        std::vector<MeshTriangle *> getMeshes() const { return _meshes; }

        void addMesh( const std::string &p_path ) { _loadFile(p_path); }
        void clearScene() { _meshes.clear(); } // clear all texutre/vertex ... from engine + clear _meshes

    private:
        void _loadFile( const std::string &p_path );
        MeshTriangle* _loadMesh( const aiMesh * const p_mesh);
        void _loadMaterial( const std::string& p_path, MeshTriangle* triMesh, const aiMaterial* const p_mtl );

    private:
        std::vector<MeshTriangle *> _meshes = std::vector<MeshTriangle *>();
    };
} // namespace Scene
} // namespace M3D
#endif
