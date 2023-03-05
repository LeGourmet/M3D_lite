#include "scene_manager.hpp"

#include "tinyGLTF/tiny_gltf.h"

#include "application.hpp"
#include "renderer/renderer.hpp"

#include <iostream>

namespace M3D
{
namespace Scene
{
    SceneManager::SceneManager() { }       
    SceneManager::~SceneManager() { clearScene(); }

    void SceneManager::loadNewScene(const std::string& p_path) { clearScene(); _loadFile(p_path); }

    void SceneManager::addCamera(std::string p_name, Camera* p_camera) { _cameras.insert(std::pair<std::string, Camera*>(p_name, p_camera)); }

    void SceneManager::addLight(std::string p_name, Light* p_light) { _lights.insert(std::pair<std::string, Light*>(p_name, p_light)); }

    void SceneManager::addMesh(std::string p_name, Mesh* p_mesh) { _meshes.insert(std::pair<std::string, Mesh*>(p_name, p_mesh)); }

    void SceneManager::addMaterial(Material* p_material) {
        _materials.push_back(p_material);
    }

    void SceneManager::addTexture(Image* p_image) {
        _textures.push_back(p_image);
        // create texture on GPU 
    }

    void SceneManager::resize(const int p_width, const int p_height) {
        for (std::pair<std::string, Camera*> val : _cameras) val.second->setScreenSize(p_width,p_height);
    }

    void SceneManager::update(unsigned long long p_deltaTime) {
        if (_mouseLeftPressed) {
            _camera.rotate(Vec3f(0.001 * _deltaMousePositionY, -0.001 * _deltaMousePositionX, 0.f));
            _deltaMousePositionX = 0;
            _deltaMousePositionY = 0;
        }
        
        Vec3f translation = VEC3F_ZERO;
        if (_isKeyPressed(SDL_SCANCODE_W) || _isKeyPressed(SDL_SCANCODE_UP)) translation.z++;
        if (_isKeyPressed(SDL_SCANCODE_S) || _isKeyPressed(SDL_SCANCODE_DOWN)) translation.z--;
        if (_isKeyPressed(SDL_SCANCODE_A) || _isKeyPressed(SDL_SCANCODE_LEFT)) translation.x++;
        if (_isKeyPressed(SDL_SCANCODE_D) || _isKeyPressed(SDL_SCANCODE_RIGHT)) translation.x--;
        if (_isKeyPressed(SDL_SCANCODE_R)) translation.y++;
        if (_isKeyPressed(SDL_SCANCODE_F)) translation.y--;
        translation *= p_deltaTime *0.01;

        _camera.move(translation);

        /*Vec3f rotation = VEC3F_ZERO;
        if (_isKeyPressed(SDL_SCANCODE_W) || _isKeyPressed(SDL_SCANCODE_UP)) rotation.x++;
        if (_isKeyPressed(SDL_SCANCODE_S) || _isKeyPressed(SDL_SCANCODE_DOWN)) rotation.x--;
        if (_isKeyPressed(SDL_SCANCODE_A) || _isKeyPressed(SDL_SCANCODE_LEFT)) rotation.y--;
        if (_isKeyPressed(SDL_SCANCODE_D) || _isKeyPressed(SDL_SCANCODE_RIGHT)) rotation.y++;
        rotation *= p_deltaTime * 0.0001;

        if(rotation != VEC3F_ZERO) _camera.rotateArround(Vec3f(0., 0., 0.), rotation);*/
    }

    bool SceneManager::captureEvent(SDL_Event p_event) { 
        Controller::KeyboardController::receiveEvent(p_event);
        Controller::MouseController::receiveEvent(p_event);
        return true; 
    }

    void SceneManager::removeCamera(std::string p_name) {
        /*Mesh* mesh = _meshes.at(p_name);
        _meshes.erase(_meshes.find(p_name));*/
        Camera* camera = _cameras.extract(p_name).mapped();
        delete camera;
    }

    void SceneManager::removeLight(std::string p_name) {
        Light* light = _lights.extract(p_name).mapped();
        delete light;
    }

    void SceneManager::removeMesh(std::string p_name) {
        Mesh* mesh = _meshes.extract(p_name).mapped();
        delete mesh;
    }

    void SceneManager::clearScene() {
        for (std::pair<std::string,Camera*> val : _cameras) removeLight(val.first);
        for (std::pair<std::string,Light*> val : _lights) removeLight(val.first);
        for (std::pair<std::string,Mesh*> val : _meshes) removeLight(val.first);
        for (int i=0; i< _materials.size() ;i++) delete _materials[i];
        for (int i=0; i< _textures.size() ;i++) delete _textures[i];
        _materials.clear();
        _textures.clear();
    }

    void SceneManager::_loadFile(const std::filesystem::path &p_path)
    {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        if (p_path.extension() == ".gltf") {
            if (!loader.LoadASCIIFromFile(&model, nullptr, nullptr, p_path.string())) throw std::runtime_error("Fail to load file: " + p_path.string());
        } else {
            if (!loader.LoadBinaryFromFile(&model, nullptr, nullptr, p_path.string())) throw std::runtime_error("Fail to load file: " + p_path.string());
        }
        
        // TinyGLTF::SetPreserveimageChannels(true) => less memory cost of texture 
        int idStartTexture = _textures.size();
        _textures.reserve(idStartTexture+model.textures.size());
        for (tinygltf::Texture t : model.textures) {
            tinygltf::Image i = model.images[t.source];
            addTexture(new Image(i.width,i.height,i.component,i.bits,i.pixel_type,i.image.data()));
        }

        for (tinygltf::Material m : model.materials){
            // ======== PbrMetallicRoughness ========
            // std::vector<double> baseColorFactor;  // len = 4. default [1,1,1,1]
            // double metallicFactor;   // default 1
            // double roughnessFactor;  // default 1
            // basecolor texture
            // metalnessRougthness texture
            
            // ======== material ========
            // std::string name;
            // std::vector<double> emissiveFactor;  // length 3. default [0, 0, 0]
            // std::string alphaMode;               // default "OPAQUE"
            // double alphaCutoff;                  // default 0.5
            //bool doubleSided;                    // default false;
            // normal texure
            // occlusion texture
            // emissive texture
        }

        //for (tinygltf::Mesh m : model.meshes)

        std::cout << "value image: " << model.images[0].image.size() << std::endl << std::endl;

        std::cout << "loaded glTF file has:\n"
            << model.accessors.size() << " accessors\n"
            //<< model.animations.size() << " animations\n"         // PAS UTILE TOUT DE SUITE
            << model.buffers.size() << " buffers\n"
            << model.bufferViews.size() << " bufferViews\n"
            << model.materials.size() << " materials\n"
            << model.meshes.size() << " meshes\n"
            << model.nodes.size() << " nodes\n"
            << model.textures.size() << " textures\n"               // DONE
            << model.images.size() << " images\n"                   // DONE
            //<< model.skins.size() << " skins\n"                   // PAS UTILE TOUT DE SUITE
            //<< model.samplers.size() << " samplers\n"             // ON VA UTILISER TOUJOURS LE MÊME OSEF
            << model.cameras.size() << " cameras\n"                 // DONE
            << model.scenes.size() << " scenes\n" 
            << model.lights.size() << " lights\n";                  // DONE



        for (tinygltf::Light l : model.lights)
            addLight(l.name, new Light(l.type, Vec3f(l.color[0], l.color[1], l.color[2]), (float)l.intensity, (float)l.spot.innerConeAngle, (float)l.spot.outerConeAngle));

        for (tinygltf::Camera c : model.cameras){
            if (c.type == "perspective") {
                _camera.setFar((float)c.perspective.zfar);
                _camera.setNear((float)c.perspective.znear);
                _camera.setFov((float)c.perspective.yfov);
            } else {

            }
            addCamera(c.name,)
        }

        // parcourir les nodes, creer graph de scene et collectioner / redistribuer les matrices M
    }
}
}
