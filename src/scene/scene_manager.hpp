#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__

#include <SDL_events.h>
#include "tinyGLTF/tiny_gltf.h"

#include "controller/keyboard_controller.hpp"
#include "controller/mouse_controller.hpp"

#include "scene_graph/scene_graph_node.hpp"

#include "camera/camera.hpp"
#include "lights/light.hpp"
#include "meshes/mesh.hpp"
#include "meshes/material.hpp"
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
        std::vector<Mesh*> getMeshes() const { return _meshes; }
        std::vector<Light*> getLights() const { return _lights; }
        Camera &getCamera() { return *_cameras[_currentCamera]; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void loadNewScene(const std::string& p_path);

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
        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile(const std::filesystem::path &p_path);
        void _createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, tinygltf::Model p_model);

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------        
        std::vector<Camera*> _cameras;
        std::vector<Light*> _lights;
        std::vector<Mesh*> _meshes;
        std::vector<Material*> _materials;
        std::vector<Image*> _textures;

        std::vector<SceneGraphNode*> _sceneGraph;
        unsigned int _currentCamera = 0;
    };
}
}
#endif
