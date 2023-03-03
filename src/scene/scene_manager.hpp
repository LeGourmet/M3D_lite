#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__

#include <assimp/material.h>
#include <assimp/scene.h>
#include <SDL.h>

#include "controller/keyboard_controller.hpp"
#include "controller/mouse_controller.hpp"
#include "meshes/mesh_triangle.hpp"
#include "camera/camera.hpp"
#include "lights/light.hpp"

#include <string>
#include <vector>

namespace M3D
{
namespace Scene
{
    class SceneManager : Controller::KeyboardController, Controller::MouseController
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        SceneManager();
        ~SceneManager();

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        std::vector<MeshTriangle *> getMeshes() const { return _meshes; }
        std::vector<Light*> getLights() const { return _lights; }
        Camera &getCamera() { return _camera; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addMeshes(const std::string& p_path);
        void addLight(Light* p_light);
        
        void update(unsigned long long p_deltaTime);
        bool captureEvent(SDL_Event p_event);

        void removeMesh(MeshTriangle* const p_mesh);
        void removeLight(Light* const p_light);
        void clearScene();
        
    private:
        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile( const std::string &p_path );
        MeshTriangle* _loadMesh( const aiMesh * const p_mesh);
        void _loadMaterial( const std::string& p_path, MeshTriangle* triMesh, const aiMaterial* const p_mtl );

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Camera _camera = Camera();
        std::vector<Light*> _lights = std::vector<Light*>();
        std::vector<MeshTriangle *> _meshes = std::vector<MeshTriangle*>();
    };
}
}
#endif
