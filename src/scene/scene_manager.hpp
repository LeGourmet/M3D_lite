#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__

#include <SDL_events.h>

#include "controller/keyboard_controller.hpp"
#include "controller/mouse_controller.hpp"

#include "camera/camera.hpp"
#include "lights/light.hpp"
#include "meshes/mesh.hpp"
#include "meshes/material.hpp"
#include "utils/image.hpp"

#include <string>
#include <map>
#include <vector>
#include <filesystem>

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
        //std::vector<MeshTriangle *> getMeshes() const { return _meshes; }
        //std::vector<Light*> getLights() const { return _lights; }
        //Camera &getCamera() { return _camera; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void loadNewScene(const std::string& p_path);
        void addCamera(std::string p_name, Camera* p_camera);
        void addLight(std::string p_name, Light* p_light);
        void addMesh(std::string p_name, Mesh* p_mesh);
        void addMaterial(Material* p_material);
        void addTexture(Image* p_image);
        
        void resize(const int p_width, const int p_height);
        void update(unsigned long long p_deltaTime);
        bool captureEvent(SDL_Event p_event);

        void removeCamera(std::string p_name);
        void removeMesh(std::string p_name);
        void removeLight(std::string p_name);
        void clearScene();
        
    private:
        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile(const std::filesystem::path &p_path);

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        // graph de scene organise les models matrix => entity représentes camera, lights ou mesh 
        std::string _currentCamera = "";
        
        std::map<std::string,Camera*> _cameras;
        std::map<std::string,Light*> _lights;
        std::map<std::string,Mesh*> _meshes;
        std::vector<Material*> _materials;
        std::vector<Image*> _textures;
    };
}
}
#endif
