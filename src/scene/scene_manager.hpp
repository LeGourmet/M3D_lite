#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__

#include <SDL_events.h>

#include "controller/keyboard_controller.hpp"
#include "controller/mouse_controller.hpp"

#include "scene_graph/scene_graph_node.hpp"

#include "objects/cameras/camera.hpp"
#include "objects/lights/light.hpp"
#include "objects/meshes/mesh.hpp"
#include "objects/meshes/material.hpp"
#include "utils/image.hpp"

#include <string>
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
        inline std::vector<Mesh*> getMeshes() const { return _meshes; }
        inline std::vector<Light*> getLights() const { return _lights; }
        inline Camera &getCamera() { return *_cameras[_currentCamera]; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void loadNewScene(const std::string& p_path);
        void addAsset(const std::string& p_path);

        void addCamera(Camera* p_camera);
        void addLight(Light* p_light);
        void addMesh(Mesh* p_mesh);
        void addMaterial(Material* p_material);
        void addTexture(Image* p_image);
        void addNode(SceneGraphNode* p_node);
        
        void resize(const int p_width, const int p_height);
        void update(unsigned long long p_deltaTime);
        bool captureEvent(SDL_Event p_event);

        void clearScene();
        
    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------        
        std::vector<Camera*> _cameras;
        std::vector<Light*> _lights;
        std::vector<Mesh*> _meshes;
        std::vector<Material*> _materials;
        std::vector<Image*> _textures;

        std::vector<SceneGraphNode*> _sceneGraph;
        unsigned int _currentCamera = 0;            // care instance camera
        
        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile(const std::filesystem::path &p_path);
        //void _createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, int* p_offsets, tinygltf::Model p_model);
    };
}
}
#endif
