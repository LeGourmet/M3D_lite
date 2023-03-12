#include "scene_manager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <fastgltf_parser.hpp>
#include <fastgltf_types.hpp>

#include "application.hpp"
#include "renderer/renderer.hpp"

#include <cmath>

#include <iostream>

namespace M3D
{
namespace Scene
{
    SceneManager::SceneManager() { }       
    SceneManager::~SceneManager() { clearScene(); }

    void SceneManager::loadNewScene(const std::string& p_path) { clearScene(); _loadFile(p_path); }

    void SceneManager::addAsset(const std::string& p_path) { _loadFile(p_path); }

    void SceneManager::addCamera(Camera* p_camera) { _cameras.push_back(p_camera); }

    void SceneManager::addLight(Light* p_light) { _lights.push_back(p_light); }

    void SceneManager::addMesh(Mesh* p_mesh) { _meshes.push_back(p_mesh); Application::getInstance().getRenderer().createMesh(p_mesh); }

    void SceneManager::addMaterial(Material* p_material) { _materials.push_back(p_material); }

    void SceneManager::addTexture(Image* p_image) { _textures.push_back(p_image); Application::getInstance().getRenderer().createTexture(p_image); }

    void SceneManager::addNode(SceneGraphNode* p_node) { _sceneGraph.push_back(p_node); }

    void SceneManager::resize(const int p_width, const int p_height) {
        for (int i=0; i<_cameras.size() ;i++) _cameras[i]->setScreenSize(p_width,p_height);
    }

    void SceneManager::update(unsigned long long p_deltaTime) {
        if (_cameras.size() != 0) {
            if (_mouseLeftPressed) {
                _cameras[_currentCamera]->rotate(Vec3f(0.001 * _deltaMousePositionY, -0.001 * _deltaMousePositionX, 0.f));
                _deltaMousePositionX = 0.;
                _deltaMousePositionY = 0.;
            }

            Vec3f translation = VEC3F_ZERO;
            if (_isKeyPressed(SDL_SCANCODE_W) || _isKeyPressed(SDL_SCANCODE_UP)) translation.z++;
            if (_isKeyPressed(SDL_SCANCODE_S) || _isKeyPressed(SDL_SCANCODE_DOWN)) translation.z--;
            if (_isKeyPressed(SDL_SCANCODE_A) || _isKeyPressed(SDL_SCANCODE_LEFT)) translation.x++;
            if (_isKeyPressed(SDL_SCANCODE_D) || _isKeyPressed(SDL_SCANCODE_RIGHT)) translation.x--;
            if (_isKeyPressed(SDL_SCANCODE_R)) translation.y++;
            if (_isKeyPressed(SDL_SCANCODE_F)) translation.y--;
            translation *= p_deltaTime * 0.01;

            _cameras[_currentCamera]->move(translation);

            /*Vec3f rotation = VEC3F_ZERO;
            if (_isKeyPressed(SDL_SCANCODE_W) || _isKeyPressed(SDL_SCANCODE_UP)) rotation.x++;
            if (_isKeyPressed(SDL_SCANCODE_S) || _isKeyPressed(SDL_SCANCODE_DOWN)) rotation.x--;
            if (_isKeyPressed(SDL_SCANCODE_A) || _isKeyPressed(SDL_SCANCODE_LEFT)) rotation.y--;
            if (_isKeyPressed(SDL_SCANCODE_D) || _isKeyPressed(SDL_SCANCODE_RIGHT)) rotation.y++;
            rotation *= p_deltaTime * 0.0001;

            if(rotation != VEC3F_ZERO) _camera.rotateArround(Vec3f(0., 0., 0.), rotation);*/
        }
    }

    bool SceneManager::captureEvent(SDL_Event p_event) { 
        Controller::KeyboardController::receiveEvent(p_event);
        Controller::MouseController::receiveEvent(p_event);
        return true; 
    }

    void SceneManager::clearScene() {
        for (int i=0; i<_meshes.size() ;i++) { Application::getInstance().getRenderer().deleteMesh(_meshes[i]); delete _meshes[i]; }
        for (int i=0; i<_textures.size() ;i++) { Application::getInstance().getRenderer().deleteTexture(_textures[i]); delete _textures[i]; }
        for (int i=0; i<_cameras.size() ;i++) delete _cameras[i];
        for (int i=0; i<_lights.size() ;i++) delete _lights[i];
        for (int i=0; i<_materials.size() ;i++) delete _materials[i];
        for (int i=0; i<_sceneGraph.size() ;i++) delete _sceneGraph[i];

        _meshes.clear();
        _textures.clear();
        _cameras.clear();
        _lights.clear();
        _materials.clear();
        _sceneGraph.clear();
    }

    /*void SceneManager::_createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, int* p_offsets, tinygltf::Model p_model) {
        SceneGraphNode* current = new SceneGraphNode();
        current->_parent = p_parent;
        if(p_model.nodes[p_idCurrent].scale.size() == 3)       current->_scale = Vec3f(glm::make_vec3(p_model.nodes[p_idCurrent].scale.data()));
        if(p_model.nodes[p_idCurrent].translation.size() == 3) current->_translation = Vec3f(glm::make_vec3(p_model.nodes[p_idCurrent].translation.data()));
        if(p_model.nodes[p_idCurrent].rotation.size())         current->_rotation = Vec4f(glm::make_vec4(p_model.nodes[p_idCurrent].rotation.data()));
        current->updateLocalTransformation();
        addNode(current);

        if      (p_model.nodes[p_idCurrent].mesh != -1)  { _meshes [p_offsets[0] + p_model.nodes[p_idCurrent].mesh  ]->addInstance(current); }
        else if (p_model.nodes[p_idCurrent].camera != -1){ _cameras[p_offsets[1] + p_model.nodes[p_idCurrent].camera]->addInstance(current); }
        else                                             { _lights [p_offsets[2] + p_model.nodes[p_idCurrent].extensions.at("KHR_lights_punctual").Get("light").GetNumberAsInt()]->addInstance(current); }

        for (int id : p_model.nodes[p_idCurrent].children)
            _createSceneGraph(id,current,p_offsets,p_model);
    }*/

    void SceneManager::_loadFile(const std::filesystem::path &p_path)
    {
        std::cout << "Start loading " << p_path << std::endl;
        fastgltf::Parser parser; // can be re-use

        fastgltf::GltfDataBuffer data; // can be re-use
        data.loadFromFile(p_path);

        std::unique_ptr<fastgltf::glTF> gltf =
            (p_path.extension() == ".gltf") ?
            parser.loadGLTF(&data, p_path.parent_path(), fastgltf::Options::None) :
            parser.loadBinaryGLTF(&data, p_path.parent_path(), fastgltf::Options::None);

        if (parser.getError() != fastgltf::Error::None) throw std::runtime_error("Fail to load file: " + p_path.string());
        if (gltf->parse() != fastgltf::Error::None) throw std::runtime_error("Fail to parse file: " + p_path.string());
        

        std::unique_ptr<fastgltf::Asset> asset = gltf->getParsedAsset();
       /* int idStartTextures = (int)_textures.size();
        _textures.reserve(idStartTextures+model.textures.size());
        for (tinygltf::Texture t : model.textures)
            addTexture(new Image(
                model.images[t.source].width,
                model.images[t.source].height,
                model.images[t.source].component,
                model.images[t.source].bits,
                model.images[t.source].pixel_type,
                model.images[t.source].image
            ));
        
        int idStartMaterials = (int)_materials.size();
        _materials.reserve(idStartMaterials + model.materials.size());
        for (tinygltf::Material m : model.materials)
            addMaterial(new Material(
                glm::make_vec4(m.pbrMetallicRoughness.baseColorFactor.data()),
                glm::make_vec3(m.emissiveFactor.data()),
                (float)m.pbrMetallicRoughness.metallicFactor,
                (float)m.pbrMetallicRoughness.roughnessFactor,
                m.alphaMode == "OPAQUE",
                (m.pbrMetallicRoughness.baseColorTexture.index == -1.) ? nullptr : _textures[idStartTextures+m.pbrMetallicRoughness.baseColorTexture.index],
                (m.pbrMetallicRoughness.metallicRoughnessTexture.index == -1.) ? nullptr : _textures[idStartTextures + m.pbrMetallicRoughness.metallicRoughnessTexture.index],
                (m.normalTexture.index == -1.) ? nullptr : _textures[idStartTextures + m.normalTexture.index],
                (m.occlusionTexture.index == -1.) ? nullptr : _textures[idStartTextures + m.occlusionTexture.index],
                (m.emissiveTexture.index == -1.) ? nullptr : _textures[idStartTextures + m.emissiveTexture.index]
            ));

        int idStartMeshes = (int)_meshes.size();
        _meshes.reserve(idStartMeshes + model.meshes.size());
        for (tinygltf::Mesh m : model.meshes) {
            Mesh* newMesh = new Mesh();
            m.primitives.reserve(m.primitives.size());
            for (tinygltf::Primitive p : m.primitives) {
                Primitive* newPrimitive = new Primitive(_materials[idStartMaterials+p.material]);

                //tinygltf::BufferView bv_position = model.bufferViews[p.attributes.at("POSITION")];
                //data = model.buffers[bv.buffer]; //memcopy

                //tinygltf::BufferView bv_normal = model.bufferViews[p.attributes.at("NORMAL")];
                //data = model.buffers[bv.buffer]; //memcopy
                
                //tinygltf::BufferView bv_texcoord = model.bufferViews[p.attributes.at("TEXCOORD_0")];
                //data = model.buffers[bv.buffer]; //memcopy

                //tinygltf::BufferView bv_indices = model.bufferViews[p.indices];
                //data = model.buffers[bv.buffer]; //memcopy

                newMesh->addPrimitive(newPrimitive);
            }
            addMesh(newMesh);
        }

        int idStartLights = (int)_lights.size();
        _lights.reserve(idStartLights + model.lights.size());
        for (tinygltf::Light l : model.lights)
            addLight(new Light(l.type, glm::make_vec4(l.color.data()), (float)l.intensity, (float)l.spot.innerConeAngle, (float)l.spot.outerConeAngle));

        int idStartCameras = (int)_cameras.size();
        _cameras.reserve(std::max(1,idStartCameras + (int)model.cameras.size()));
        for (tinygltf::Camera c : model.cameras)
            if (c.type == "perspective")
                addCamera(new Camera((float)c.perspective.znear, (float)c.perspective.zfar, (float)c.perspective.yfov));

        if (_cameras.empty()) addCamera(new Camera(1., 1000., 0.5));
        _currentCamera = 0;
       
        int idStartSceneGraph = (int)_sceneGraph.size();
        _sceneGraph.reserve(idStartSceneGraph + model.nodes.size());
        int offsets[] = {idStartMeshes,idStartCameras,idStartLights};
        for (tinygltf::Scene s : model.scenes)
            for (int id : s.nodes)
                _createSceneGraph(id, nullptr, offsets, model);*/

        std::cout << "Finished to load " << p_path << std::endl;
    }
}
}
