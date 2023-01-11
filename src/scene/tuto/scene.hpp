#ifndef __MODEL_SCENE__
#define __MODEL_SCENE__

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
        void clearScene();

    private:
        void _loadFile( const std::string &p_path );
        void _loadMesh( const aiMesh * const p_mesh, const aiScene * const p_scene );
		void _loadMaterial( const aiMaterial * const p_mtl );
		Texture	 _loadTexture( const aiString & p_path, const std::string & p_type );

    private:
        std::vector<MeshTriangle *> _meshes = std::vector<MeshTriangle *>();
    };
} // namespace Scene
} // namespace M3D
#endif
