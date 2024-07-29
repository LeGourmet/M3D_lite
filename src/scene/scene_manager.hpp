#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__

#include <SDL_events.h>

#include "controller/keyboard_controller.hpp"
#include "controller/mouse_controller.hpp"

#include "utils/image.hpp"
#include "utils/define.hpp"

#include "objects/lights/light.hpp"
#include "objects/meshes/mesh.hpp"
#include "objects/meshes/texture.hpp"
#include "objects/meshes/material.hpp"
#include "objects/cameras/camera.hpp"
#include "scene_graph_node.hpp"

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
            SceneManager(const int p_width, const int p_height);
            ~SceneManager();

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            inline const std::vector<Mesh>& getMeshes() const { return _meshes; }
            inline const std::vector<Light>& getLights() const { return _lights; }
            inline const std::vector<Camera>& getCameras() const { return _cameras; }

            inline const float getMovementSensitivity() const { return _movementSensitivity; }
            inline const float getRotationSensitivity() const { return _rotationSensitivity; }

            // sure => todo virer ou coder mieu
            Camera& getMainCamera();
            SceneGraphNode* getMainCameraSceneGraphNode();
            const Mat4f getMainCameraTransformation();
            const Mat4f getMainCameraViewMatrix();
            const Mat4f getMainCameraProjectionMatrix();

            // ----------------------------------------------------- SETTERS -------------------------------------------------------
            void setMovementSensitivity(float p_value);
            void setRotationSensitivity(float p_value);

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void loadNewScene(const std::filesystem::path& p_path);
            void addAsset(const std::filesystem::path& p_path);

            void addCamera(Camera p_camera);
            void addLight(Light p_light);
            void addMesh(Mesh p_mesh);
            void addMaterial(Material p_material);
            void addTexture(Texture p_texture);
            void addImage(Image p_image);
            void addNode(SceneGraphNode p_node);

            void addInstance(Camera& p_camera, SceneGraphNode* p_node);
            void addInstance(Light& p_light, SceneGraphNode* p_node);
            void addInstance(Mesh& p_mesh, SceneGraphNode* p_node);

            void resize(const int p_width, const int p_height);
            void update(float p_deltaTime);
            bool captureEvent(const SDL_Event& p_event);
        
            void clearEvents();
            void clearScene();
        
        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------   
            std::vector<Camera>    _cameras;
            std::vector<Light>     _lights;
            std::vector<Mesh>      _meshes;
            std::vector<Material>  _materials;
            std::vector<Texture>   _textures;
            std::vector<Image>     _images;
            // skining and animation
            // env map
            // dirty ?

            float _movementSensitivity = 1.f;
            float _rotationSensitivity = 0.1f;

            std::vector<SceneGraphNode> _sceneGraphNodes;
            Vec2i _mainCamera;
        
            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void _loadFile(const std::filesystem::path& p_path);
        };
    }
}

#endif
