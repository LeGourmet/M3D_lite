#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <assimp/material.h>
#include <assimp/scene.h>
#include <SDL.h>

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
        void addMeshes( const std::string &p_path ) { _loadFile(p_path); }
        //void addMesh( const std::string& p_path, const std::string& p_name) { } // load only one meshes from obj
        void update(const float p_deltaTime) {}
        bool captureEvent(SDL_Event p_event) { return false; }
        void removeMesh( MeshTriangle * const p_mesh ) { _meshes.erase( std::find( _meshes.begin(), _meshes.end(), p_mesh ) ); }
        void clearScene() { _meshes.clear(); _camera->reset(); } // + clear all texutre/vertex ... from engine + clear _meshes
        
    private:
        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile( const std::string &p_path );
        MeshTriangle* _loadMesh( const aiMesh * const p_mesh);
        void _loadMaterial( const std::string& p_path, MeshTriangle* triMesh, const aiMaterial* const p_mtl );

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Camera *_camera;
        std::vector<MeshTriangle *> _meshes = std::vector<MeshTriangle *>();      
    };
}
}
#endif
