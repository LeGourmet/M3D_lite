#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__

#include <SDL_events.h>
//#include <fastgltf_parser.hpp>
//#include <fastgltf_types.hpp>
#include "tinygltf/tiny_gltf.h"

#include "controller/keyboard_controller.hpp"
#include "controller/mouse_controller.hpp"

#include "utils/image.hpp"
#include "utils/texture.hpp"
#include "utils/define.hpp"

#include <vector>
#include <filesystem>

namespace M3D
{
    namespace Scene
    {
        class Camera;
        class Light;
        class Mesh;
        class Material;
        class SceneGraphNode;
    }

namespace Scene
{
    class SceneManager : Controller::KeyboardController, Controller::MouseController
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        SceneManager(const int p_width, const int p_height);
        ~SceneManager();

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        inline const std::vector<Mesh*>& getMeshes() const { return _meshes; }
        inline const std::vector<Light*>& getLights() const { return _lights; }
        inline const std::vector<Camera*>& getCameras() const { return _cameras; }

        Camera &getMainCamera() const;
        SceneGraphNode* getMainCameraSceneGraphNode() const;
        const Mat4f getMainCameraTransformation() const;
        const Mat4f getMainCameraViewMatrix() const;
        const Mat4f getMainCameraProjectionMatrix() const;

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void loadNewScene(const std::string& p_path);
        void addAsset(const std::string& p_path);

        void addCamera(Camera* p_camera);
        void addInstance(Camera* p_camera, SceneGraphNode* p_node);
        void addLight(Light* p_light);
        void addInstance(Light* p_light, SceneGraphNode* p_node);
        void addMesh(Mesh* p_mesh);
        void addInstance(Mesh* p_mesh, SceneGraphNode* p_node);
        void addMaterial(Material* p_material);
        void addTexture(Texture* p_texture);
        void addImage(Image* p_image);
        void addNode(SceneGraphNode* p_node);

        void resize(const int p_width, const int p_height);
        void update(unsigned long long p_deltaTime);
        bool captureEvent(const SDL_Event& p_event);
        
        void clearEvents();
        void clearScene();
        
    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------        
        std::vector<Camera*>    _cameras;
        std::vector<Light*>     _lights;
        std::vector<Mesh*>      _meshes;
        std::vector<Material*>  _materials;
        std::vector<Texture*>   _textures;
        std::vector<Image*>     _images;

        std::vector<SceneGraphNode*> _sceneGraph;
        Vec2i _mainCamera;
        
        //fastgltf::Parser _parser = fastgltf::Parser(fastgltf::Extensions::KHR_lights_punctual);

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void _loadFile(const std::filesystem::path &p_path);
        //void _createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, unsigned int p_meshOffset, unsigned int p_lightOffset, unsigned int p_camOffset, std::vector<fastgltf::Node>& p_nodes);
        void _createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, unsigned int p_meshOffset, unsigned int p_lightOffset, unsigned int p_camOffset, tinygltf::Model &p_model);
    };
}
}
#endif
