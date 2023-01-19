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
        SceneManager();
        ~SceneManager();

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        std::vector<MeshTriangle *> getMeshes() const { return _meshes; }
        Camera& getCamera() const { return *_camera; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addMeshes(const std::string& p_path);
        void addMesh(const std::string& p_path, const std::string& p_name);
        
        void update(const float p_deltaTime);
        bool captureEvent(SDL_Event p_event);

        void removeMesh(const unsigned int p_id);
        void removeMesh(MeshTriangle* const p_mesh);
        void clearScene();
        
    private:
        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile( const std::string &p_path );
        MeshTriangle* _loadMesh( const aiMesh * const p_mesh);
        void _loadMaterial( const std::string& p_path, MeshTriangle* triMesh, const aiMaterial* const p_mtl );

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Camera *_camera;
        // lights ?
        std::vector<MeshTriangle *> _meshes;      
    };
}
}
#endif