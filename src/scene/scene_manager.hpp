#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <assimp/material.h>
#include <assimp/scene.h>

#include "mesh_triangle.hpp"
#include "camera.hpp"

#include <string>
#include <vector>

namespace M3D
{
namespace Scene
{
    class SceneManager
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        SceneManager() { _camera = new Camera(); }
        ~SceneManager() { 
            clearScene();
            delete _camera; 
        }

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        std::vector<MeshTriangle *> getMeshes() const { return _meshes; }
        Camera& getCamera() const { return *_camera; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addMesh( const std::string &p_path ) { _loadFile(p_path); }
        void clearScene() { _meshes.clear(); _camera->reset(); } // + clear all texutre/vertex ... from engine + clear _meshes

    private:
        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile( const std::string &p_path );
        MeshTriangle* _loadMesh( const aiMesh * const p_mesh);
        void _loadMaterial( const std::string& p_path, MeshTriangle* triMesh, const aiMaterial* const p_mtl );

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        std::vector<MeshTriangle *> _meshes = std::vector<MeshTriangle *>();      
        Camera* _camera;
    };
}
}
#endif
