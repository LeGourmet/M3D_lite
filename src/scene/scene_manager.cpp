#include "scene_manager.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "application.hpp"
#include "renderer/renderer.hpp"

#include "scene_graph/scene_graph_node.hpp"

#include "objects/lights/light.hpp"
#include "objects/meshes/mesh.hpp"
#include "objects/meshes/material.hpp"
#include "objects/cameras/camera.hpp"

//#include <variant>
//#include <optional>
#include <iostream>

namespace M3D
{
namespace Scene
{
    SceneManager::SceneManager(const int p_width, const int p_height) {
        addMaterial(new Material(VEC4F_ONE, VEC3F_ZERO, 0.f, 0.f, 1.f, 1.f, false, nullptr, nullptr, nullptr, nullptr, nullptr));
        
        addCamera(new Camera(PIf/2.f, 1., 1e-2f, 1e3f ,CAMERA_TYPE::PERSPECTIVE));
        addNode(new SceneGraphNode(nullptr,VEC3F_ZERO,VEC3F_ONE,QUATF_ID));
        addInstance(_cameras[0], _sceneGraph[0]);
        _mainCamera = Vec2i(0, 0);

        resize(p_width, p_height);
    }

    SceneManager::~SceneManager() { 
        for (int i=0; i<_cameras.size() ;i++) delete _cameras[i];
        for (int i=0; i<_materials.size() ;i++) delete _materials[i];
        for (int i=0; i<_sceneGraph.size() ;i++) delete _sceneGraph[i];
        for (int i=0; i<_meshes.size() ;i++) { Application::getInstance().getRenderer().deleteMesh(_meshes[i]); delete _meshes[i]; }
        for (int i=0; i<_textures.size() ;i++) { Application::getInstance().getRenderer().deleteTexture(_textures[i]); delete _textures[i]; }
        for (int i = 0; i<_images.size();i++) { delete _images[i]; }
        for (int i=0; i<_lights.size() ;i++) delete _lights[i];
    }

    Camera &SceneManager::getMainCamera() const { return *_cameras[_mainCamera.x]; }
    SceneGraphNode* SceneManager::getMainCameraSceneGraphNode() const { return _cameras[_mainCamera.x]->getInstance(_mainCamera.y); }
    const Mat4f SceneManager::getMainCameraTransformation() const { return _cameras[_mainCamera.x]->getInstance(_mainCamera.y)->getTransformation(); }
    const Mat4f SceneManager::getMainCameraViewMatrix() const { return _cameras[_mainCamera.x]->getViewMatrix(_mainCamera.y); }
    const Mat4f SceneManager::getMainCameraProjectionMatrix() const { return _cameras[_mainCamera.x]->getProjectionMatrix(); }

    void SceneManager::loadNewScene(const std::string& p_path) { clearScene(); _loadFile(p_path); }

    void SceneManager::addAsset(const std::string& p_path) { _loadFile(p_path); }

    void SceneManager::addCamera(Camera* p_camera) { _cameras.push_back(p_camera); }

    void SceneManager::addLight(Light* p_light) { _lights.push_back(p_light); }

    void SceneManager::addMesh(Mesh* p_mesh) { _meshes.push_back(p_mesh); Application::getInstance().getRenderer().createMesh(p_mesh); }

    void SceneManager::addMaterial(Material* p_material) { _materials.push_back(p_material); }

    void SceneManager::addTexture(Texture* p_texture) { _textures.push_back(p_texture); Application::getInstance().getRenderer().createTexture(p_texture); }

    void SceneManager::addImage(Image* p_image) { _images.push_back(p_image); }

    void SceneManager::addNode(SceneGraphNode* p_node) { _sceneGraph.push_back(p_node); }

    void SceneManager::resize(const int p_width, const int p_height) { for (int i=0; i<_cameras.size() ;i++) _cameras[i]->setScreenSize(p_width,p_height); }

    void SceneManager::update(unsigned long long p_deltaTime) {
        SceneGraphNode* cameraInstance  = _cameras[_mainCamera.x]->getInstance(_mainCamera.y);
        
        Vec3f rotation = VEC3F_ZERO;
        Vec3f translation = VEC3F_ZERO;
        
        if (_mouseLeftPressed) {
            rotation += Vec3f(_deltaMousePositionY, -_deltaMousePositionX, 0.f);
            _deltaMousePositionX = 0.;
            _deltaMousePositionY = 0.;
        }

        if (_isKeyPressed(SDL_SCANCODE_W) || _isKeyPressed(SDL_SCANCODE_UP)) translation += cameraInstance->getFront();
        if (_isKeyPressed(SDL_SCANCODE_S) || _isKeyPressed(SDL_SCANCODE_DOWN)) translation += cameraInstance->getBack();
        if (_isKeyPressed(SDL_SCANCODE_A) || _isKeyPressed(SDL_SCANCODE_LEFT)) translation += cameraInstance->getLeft();
        if (_isKeyPressed(SDL_SCANCODE_D) || _isKeyPressed(SDL_SCANCODE_RIGHT)) translation += cameraInstance->getRight();
        if (_isKeyPressed(SDL_SCANCODE_R)) translation += cameraInstance->getUp();
        if (_isKeyPressed(SDL_SCANCODE_F)) translation += cameraInstance->getDown();
        /*if (_isKeyPressed(SDL_SCANCODE_W) || _isKeyPressed(SDL_SCANCODE_UP)) rotation.z++;// left
        if (_isKeyPressed(SDL_SCANCODE_S) || _isKeyPressed(SDL_SCANCODE_DOWN)) rotation.z--;
        if (_isKeyPressed(SDL_SCANCODE_A) || _isKeyPressed(SDL_SCANCODE_LEFT)) rotation.x++;//up
        if (_isKeyPressed(SDL_SCANCODE_D) || _isKeyPressed(SDL_SCANCODE_RIGHT)) rotation.x--;
        if (_isKeyPressed(SDL_SCANCODE_R)) translation.y++; // front
        if (_isKeyPressed(SDL_SCANCODE_F)) translation.y--;*/
            
        rotation *= p_deltaTime * 0.0001;
        translation *= p_deltaTime * 0.001;

        cameraInstance->translate(translation);
        cameraInstance->rotate(rotation);

        // todo add dirty flag to not always update matrices
        for (Mesh* mesh : _meshes)
            for (unsigned int i=0; i<mesh->getNumberInstances() ;i++)
                Application::getInstance().getRenderer().updateInstanceMesh(
                    mesh, i,
                    mesh->getInstance(i)->getTransformation(),
                    _cameras[_mainCamera.x]->getViewMatrix(_mainCamera.y),
                    _cameras[_mainCamera.x]->getProjectionMatrix()
                );
    }

    bool SceneManager::captureEvent(const SDL_Event& p_event) { 
        Controller::KeyboardController::receiveEvent(p_event);
        Controller::MouseController::receiveEvent(p_event);
        return true; 
    }

    void SceneManager::clearEvents() { 
        KeyboardController::clearEvents(); 
        MouseController::clearEvents();
    }

    void SceneManager::clearScene() {
        for (int i=1; i<_cameras.size() ;i++) delete _cameras[i];
        for (int i=1; i<_materials.size() ;i++) delete _materials[i];
        for (int i=1; i<_sceneGraph.size() ;i++) delete _sceneGraph[i];
        _cameras.erase(_cameras.begin()+1,_cameras.end());
        _materials.erase(_materials.begin()+1,_materials.end());
        _sceneGraph.erase(_sceneGraph.begin()+1,_sceneGraph.end());

        for (int i = 0; i < _meshes.size();i++) { Application::getInstance().getRenderer().deleteMesh(_meshes[i]); delete _meshes[i]; }
        for (int i = 0; i < _textures.size();i++) { Application::getInstance().getRenderer().deleteTexture(_textures[i]); delete _textures[i]; }
        for (int i = 0; i < _images.size();i++) delete _images[i];
        for (int i = 0; i < _lights.size();i++) delete _lights[i];
        _meshes.clear();
        _textures.clear();
        _images.clear();
        _lights.clear();

        _sceneGraph[0]->clearChilds();
        _mainCamera = Vec2i(0, 0); // care need to reset pos
    }

    void SceneManager::addInstance(Camera* p_camera, SceneGraphNode* p_node) { p_camera->addInstance(p_node); }

    void SceneManager::addInstance(Light* p_light, SceneGraphNode* p_node) { p_light->addInstance(p_node); }

    void SceneManager::addInstance(Mesh* p_mesh, SceneGraphNode* p_node) {
        p_mesh->addInstance(p_node);

        Application::getInstance().getRenderer().addInstanceMesh(
            p_mesh,
            p_node->getTransformation(),
            _cameras[_mainCamera.x]->getViewMatrix(_mainCamera.y),
            _cameras[_mainCamera.x]->getProjectionMatrix()
        );
    }

    void SceneManager::_createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, unsigned int p_meshOffset, unsigned int p_lightOffset, unsigned int p_camOffset, tinygltf::Model& p_model) {
        Vec3f translation = (p_model.nodes[p_idCurrent].translation.size() == 3) ? (Vec3f)glm::make_vec3(p_model.nodes[p_idCurrent].translation.data()) : VEC3F_ZERO;
        Vec3f scale = (p_model.nodes[p_idCurrent].scale.size() == 3) ? (Vec3f)glm::make_vec3(p_model.nodes[p_idCurrent].scale.data()) : VEC3F_ONE;
        Quatf rotation = (p_model.nodes[p_idCurrent].rotation.size() == 4) ? Quatf((float)p_model.nodes[p_idCurrent].rotation[3], (float)p_model.nodes[p_idCurrent].rotation[0], (float)p_model.nodes[p_idCurrent].rotation[1], (float)p_model.nodes[p_idCurrent].rotation[2]) : QUATF_ID;
        SceneGraphNode* current = new SceneGraphNode(p_parent, translation, scale, rotation);
        addNode(current);

        if (p_model.nodes[p_idCurrent].mesh != -1) { addInstance(_meshes[p_meshOffset + p_model.nodes[p_idCurrent].mesh], current); }
        else if (p_model.nodes[p_idCurrent].camera != -1) { addInstance(_cameras[p_camOffset + p_model.nodes[p_idCurrent].camera], current); }
        else if (p_model.nodes[p_idCurrent].skin != -1) {}
        else if (p_model.nodes[p_idCurrent].extensions.find("KHR_lights_punctual") != p_model.nodes[p_idCurrent].extensions.end() &&
                 p_model.nodes[p_idCurrent].extensions.at("KHR_lights_punctual").Has("light")) {
            addInstance(_lights[p_lightOffset + p_model.nodes[p_idCurrent].extensions.at("KHR_lights_punctual").Get("light").GetNumberAsInt()], current); 
        }

        for (int id : p_model.nodes[p_idCurrent].children)
            _createSceneGraph(id, current, p_meshOffset, p_lightOffset, p_camOffset, p_model);
    }

    void SceneManager::_loadFile(const std::filesystem::path& p_path)
    {
        std::cout << "Start loading " << p_path << std::endl;

        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        loader.SetPreserveImageChannels(true);

        if (p_path.extension() == ".gltf") {
            if (!loader.LoadASCIIFromFile(&model, nullptr, nullptr, p_path.string())) throw std::runtime_error("Fail to load file: " + p_path.string());
        }
        else {
            if (!loader.LoadBinaryFromFile(&model, nullptr, nullptr, p_path.string())) throw std::runtime_error("Fail to load file: " + p_path.string());
        }
        std::cout << "tiny finish"<< std::endl;

        // ------------- IMAGES
        unsigned int startIdImages = (unsigned int)_images.size();
        _images.reserve(startIdImages + model.images.size());
        for (tinygltf::Image i : model.images)
            addImage(new Image(i.width, i.height, i.component, i.bits, i.pixel_type, i.image));

        std::cout << "images loaded: " << _images.size() - startIdImages << std::endl;

        // ------------- TEXTURES ==> texture_ogl very slow ! and memory leak ?? 
        unsigned int startIdTextures = (unsigned int)_textures.size();
        _textures.reserve(startIdTextures + model.textures.size());
        for (tinygltf::Texture t : model.textures) {
            Texture* texture = new Texture();
            texture->_image = _images[startIdImages + t.source];

            switch (model.samplers[t.sampler].magFilter) {
            case TINYGLTF_TEXTURE_FILTER_NEAREST: texture->_magnification = MAGNIFICATION_TYPE::MAG_NEAREST; break;
            default: texture->_magnification = MAGNIFICATION_TYPE::MAG_LINEAR; break;
            }

            switch (model.samplers[t.sampler].minFilter) {
            case TINYGLTF_TEXTURE_FILTER_NEAREST: texture->_minification = MINIFICATION_TYPE::MIN_NEAREST; break;
            case TINYGLTF_TEXTURE_FILTER_LINEAR: texture->_minification = MINIFICATION_TYPE::MIN_LINEAR; break;
            case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST: texture->_minification = MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_NEAREST; break;
            case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST: texture->_minification = MINIFICATION_TYPE::MIN_LINEAR_MIPMAP_NEAREST; break;
            case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR: texture->_minification = MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_LINEAR; break;
            default: texture->_minification = MINIFICATION_TYPE::MIN_LINEAR_MIPMAP_LINEAR; break;
            }

            switch (model.samplers[t.sampler].wrapR) {
            case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: texture->_wrappingR = WRAPPING_TYPE::WRAP_CLAMP_TO_EDGE; break;
            case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: texture->_wrappingR = WRAPPING_TYPE::WRAP_MIRRORED_REPEAT; break;
            default: texture->_wrappingR = WRAPPING_TYPE::WRAP_REPEAT; break;
            }

            switch (model.samplers[t.sampler].wrapS) {
            case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: texture->_wrappingS = WRAPPING_TYPE::WRAP_CLAMP_TO_EDGE; break;
            case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: texture->_wrappingS = WRAPPING_TYPE::WRAP_MIRRORED_REPEAT; break;
            default: texture->_wrappingS = WRAPPING_TYPE::WRAP_REPEAT; break;
            }

            switch (model.samplers[t.sampler].wrapT) {
            case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: texture->_wrappingT = WRAPPING_TYPE::WRAP_CLAMP_TO_EDGE; break;
            case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: texture->_wrappingT = WRAPPING_TYPE::WRAP_MIRRORED_REPEAT; break;
            default: texture->_wrappingT = WRAPPING_TYPE::WRAP_REPEAT; break;
            }

            addTexture(texture);
        }

        std::cout << "textures loaded: " << _textures.size() - startIdTextures << std::endl;

        // ------------- MATERIALS
        unsigned int startIdMaterials = (unsigned int)_materials.size();
        _materials.reserve(startIdMaterials + model.materials.size());
        for (tinygltf::Material m : model.materials)
            addMaterial(new Material(
                glm::make_vec4(m.pbrMetallicRoughness.baseColorFactor.data()),
                glm::make_vec3(m.emissiveFactor.data()),
                m.extensions.find("KHR_materials_emissive_strength") != m.extensions.end() && m.extensions.at("KHR_materials_emissive_strength").Has("emissiveStrength") ? (float)m.extensions.at("KHR_materials_emissive_strength").Get("emissiveStrength").GetNumberAsDouble() : 0.f,
                (float)m.pbrMetallicRoughness.metallicFactor,
                (float)m.pbrMetallicRoughness.roughnessFactor,
                (float)(m.alphaMode == "OPAQUE" ? 0. : (m.alphaMode == "BLEND" ? 1. : m.alphaCutoff)),
                m.doubleSided,
                (m.pbrMetallicRoughness.baseColorTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.pbrMetallicRoughness.baseColorTexture.index],
                (m.pbrMetallicRoughness.metallicRoughnessTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.pbrMetallicRoughness.metallicRoughnessTexture.index],
                (m.normalTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.normalTexture.index],
                (m.occlusionTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.occlusionTexture.index],
                (m.emissiveTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.emissiveTexture.index]
            ));

        std::cout << "materials loaded: " << _materials.size() - startIdMaterials << std::endl;

        // ------------- MESHES
        unsigned int startIdMeshes = (unsigned int)_meshes.size();
        _meshes.reserve(startIdMeshes + model.meshes.size());
        for (tinygltf::Mesh m : model.meshes) {
            Mesh* newMesh = new Mesh();
            newMesh->getPrimitives().reserve(m.primitives.size());
            for (tinygltf::Primitive p : m.primitives) {
                if (p.indices == -1) throw std::runtime_error("Fail to load file: primitive indices must be define.");

                Primitive* newPrimitive = new Primitive(_materials[startIdMaterials + p.material]); // care default material

                tinygltf::Accessor a_position = model.accessors[p.attributes.at("POSITION")];
                tinygltf::Accessor a_normal = model.accessors[p.attributes.at("NORMAL")];
                tinygltf::Accessor a_texcoord = model.accessors[p.attributes.at("TEXCOORD_0")];

                if (!((a_position.count == a_normal.count) && (a_normal.count == a_texcoord.count))) throw std::runtime_error("Fail to load file: primitive vertices must have the same number of position, normal and texcoord0.");

                tinygltf::BufferView bv_position = model.bufferViews[a_position.bufferView];
                const float* positionsBuffer = reinterpret_cast<const float*>(&model.buffers[bv_position.buffer].data[a_position.byteOffset + bv_position.byteOffset]);

                tinygltf::BufferView bv_normal = model.bufferViews[a_normal.bufferView];
                const float* normalsBuffer = reinterpret_cast<const float*>(&model.buffers[bv_normal.buffer].data[a_normal.byteOffset + bv_normal.byteOffset]);

                tinygltf::Accessor a_indices = model.accessors[p.indices];
                tinygltf::BufferView bv_texcoord = model.bufferViews[a_texcoord.bufferView];
                const float* texCoord0Buffer = reinterpret_cast<const float*>(&model.buffers[bv_texcoord.buffer].data[a_texcoord.byteOffset + bv_texcoord.byteOffset]);

                newPrimitive->getVertices().reserve(a_position.count);
                for (unsigned int i = 0; i < a_position.count;i++) {
                    Vertex v = Vertex{
                                        ._position = glm::make_vec3(&positionsBuffer[i * 3]),
                                        ._normal = glm::normalize(glm::make_vec3(&normalsBuffer[i * 3])),
                                        ._uv = glm::make_vec2(&texCoord0Buffer[i * 2])
                    };
                    v._tangent = glm::cross(v._normal, VEC3F_X);
                    if (glm::length(v._tangent) < 0.1) v._tangent = glm::cross(v._normal, VEC3F_Y);
                    v._tangent = glm::normalize(v._tangent);
                    v._bitangent = glm::normalize(glm::cross(v._normal, v._tangent));
                    newPrimitive->addVertex(v);
                }

                tinygltf::BufferView bv_indices = model.bufferViews[a_indices.bufferView];
                switch (a_indices.componentType) {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                    newPrimitive->setIndices(reinterpret_cast<const unsigned int*>(&model.buffers[bv_indices.buffer].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                    break;
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    newPrimitive->setIndices(reinterpret_cast<const unsigned short*>(&model.buffers[bv_indices.buffer].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                    break;
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    newPrimitive->setIndices(reinterpret_cast<const unsigned char*>(&model.buffers[bv_indices.buffer].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                    break;
                }

                newMesh->addPrimitive(newPrimitive);
            }
            addMesh(newMesh);
        }

        std::cout << "meshes loaded: " << _meshes.size() - startIdMeshes << std::endl;

        // ------------- LIGHTS
        unsigned int startIdLights = (unsigned int)_lights.size();
        _lights.reserve(startIdLights + model.lights.size());
        for (tinygltf::Light l : model.lights) {
            if (l.type == "spot") {
                addLight(new Light(LIGHT_TYPE::SPOT, ((l.color.size() != 3) ? VEC3F_ONE : (Vec3f)glm::make_vec3(l.color.data())), (float)l.intensity, (float)l.spot.innerConeAngle, (float)l.spot.outerConeAngle));
            }
            else {
                addLight(new Light((l.type == "point" ? LIGHT_TYPE::POINT : LIGHT_TYPE::DIRECTIONAL), ((l.color.size() != 3) ? VEC3F_ONE : (Vec3f)glm::make_vec3(l.color.data())), (float)l.intensity));
            }
        }

        std::cout << "lights loaded: " << _lights.size() - startIdLights << std::endl;

        // ------------- CAMERAS
        int startIdCameras = (int)_cameras.size();
        _cameras.reserve(startIdCameras + (int)model.cameras.size());
        for (tinygltf::Camera c : model.cameras) {
            if (c.type == "perspective") {  // CHECK use aspect ratio ??
                addCamera(new Camera((float)c.perspective.yfov, float(Application::getInstance().getWidth())/float(Application::getInstance().getHeight()), (float)c.perspective.znear, (float)c.perspective.zfar, CAMERA_TYPE::PERSPECTIVE));
            }
            else if (c.type == "orthographic") { // CHECK multiply by real aspect ratio ?
                addCamera(new Camera((float)c.orthographic.xmag, (float)c.orthographic.ymag, (float)c.orthographic.znear, (float)c.orthographic.zfar, CAMERA_TYPE::ORTHOGRAPHIC));
            }
        }

        std::cout << "cameras loaded: " << _cameras.size() - startIdCameras << std::endl;
        
        // ------------- SCENE GRAPH
        unsigned int startIdSceneGraph = (unsigned int)_sceneGraph.size();
        _sceneGraph.reserve(startIdSceneGraph + model.nodes.size());
        for (tinygltf::Scene s : model.scenes)
            for (int id : s.nodes)
                _createSceneGraph(id, nullptr, startIdMeshes, startIdLights, startIdCameras, model);

        std::cout << "nodes loaded: " << _sceneGraph.size() - startIdSceneGraph << std::endl;

        if (_cameras.size() > 1 && _cameras[1]->getNumberInstances() > 0) _mainCamera = Vec2i(1, 0);

        std::cout << "Finished to load " << p_path << std::endl;
    }

    /*void SceneManager::_createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, unsigned int p_meshOffset, unsigned int p_lightOffset, unsigned int p_camOffset, std::vector<fastgltf::Node>& p_nodes) {
        if(!std::holds_alternative<fastgltf::Node::TRS>(p_nodes[p_idCurrent].transform)) throw std::runtime_error("Transform matrix need to be decompose!");
        fastgltf::Node::TRS transform = std::get<0>(p_nodes[p_idCurrent].transform);
   
        Vec3f translation   = (Vec3f)glm::make_vec3(transform.translation.data());
        Vec3f scale         = (Vec3f)glm::make_vec3(transform.scale.data());
        Quatf rotation      = Quatf((float)transform.rotation[3], (float)transform.rotation[0], (float)transform.rotation[1], (float)transform.rotation[2]);
        SceneGraphNode* current = new SceneGraphNode(p_parent, translation, scale, rotation);
        addNode(current);

        if (p_nodes[p_idCurrent].meshIndex.has_value())         { addInstance(_meshes[p_meshOffset + p_nodes[p_idCurrent].meshIndex.value()], current); }
        else if (p_nodes[p_idCurrent].cameraIndex.has_value())  { addInstance(_cameras[p_camOffset + p_nodes[p_idCurrent].cameraIndex.value()], current); }
        else if (p_nodes[p_idCurrent].skinIndex.has_value())    {}
        else if (p_nodes[p_idCurrent].lightsIndex.has_value())  { addInstance(_lights[p_lightOffset + p_nodes[p_idCurrent].lightsIndex.value()], current); }

        for (int id : p_nodes[p_idCurrent].children)
            _createSceneGraph(id, current, p_meshOffset, p_lightOffset, p_camOffset, p_nodes);
    }*/

    /*void SceneManager::_loadFile(const std::filesystem::path& p_path)
    {
        std::cout << "Start loading " << p_path << std::endl;

        fastgltf::GltfDataBuffer data;
        data.loadFromFile(p_path);

        std::cout << "load file!" << std::endl;

        std::unique_ptr<fastgltf::glTF> gltf =
            (p_path.extension() == ".gltf") ? // test option => DontRequireValidAssetMember || LoadGLBBuffers || LoadExternalBuffers
            _parser.loadGLTF(&data, p_path.parent_path(), fastgltf::Options::DecomposeNodeMatrices ) :
            _parser.loadBinaryGLTF(&data, p_path.parent_path(), fastgltf::Options::DecomposeNodeMatrices );

        if (_parser.getError() != fastgltf::Error::None) throw std::runtime_error("Fail to load file: " + p_path.string());
        if (gltf->parse() != fastgltf::Error::None) throw std::runtime_error("Fail to parse file: " + p_path.string());    // on peut cibler le parse (-skins/-animations)

        std::unique_ptr<fastgltf::Asset> asset = gltf->getParsedAsset();

        // ------------- IMAGES
        unsigned int startIdImages = (unsigned int)_images.size();
        _images.reserve(startIdImages + asset->images.size());
        for (fastgltf::Image i : asset->images) {
            fastgltf::DataSource dataSource = i.data;
            if (std::holds_alternative<std::monostate>(dataSource) || std::holds_alternative<fastgltf::sources::CustomBuffer>(dataSource)) throw std::runtime_error("Image data source invalid!"); // change for better
            if (std::holds_alternative<fastgltf::sources::BufferView>(dataSource)) {} //addTexture(new Image()); parse data
            if (std::holds_alternative<fastgltf::sources::FilePath>(dataSource)) {} //addTexture(new Image()); use stb to parse
            if (std::holds_alternative<fastgltf::sources::Vector>(dataSource)) {} //addTexture(new Image()); memcpy ou use pointer + mime type for datas
            //addImage(new Image(i.width, i.height, i.component, i.bits, i.pixel_type, i.image));
        }

        // ------------- TEXTURES
        unsigned int startIdTextures = (unsigned int)_textures.size();
        _textures.reserve(startIdTextures + asset->textures.size());
        for (fastgltf::Texture t : asset->textures) {
            Texture* texture = new Texture();
            if(!t.imageIndex.has_value()) throw std::runtime_error("Texture: invalud image index!");
            texture->_image = _images[startIdImages + t.imageIndex.value()];

            if (t.samplerIndex.has_value()) {
                if (asset->samplers[t.samplerIndex.value()].magFilter.has_value()) {
                    switch (asset->samplers[t.samplerIndex.value()].magFilter.value()) { // optional
                    case fastgltf::Filter::Nearest: texture->_magnification = MAGNIFICATION_TYPE::MAG_NEAREST; break;
                    default: texture->_magnification = MAGNIFICATION_TYPE::MAG_LINEAR; break;
                    }
                }

                if (asset->samplers[t.samplerIndex.value()].minFilter.has_value()) {
                    switch (asset->samplers[t.samplerIndex.value()].minFilter.value()) {
                    case fastgltf::Filter::Nearest: texture->_minification = MINIFICATION_TYPE::MIN_NEAREST; break;
                    case fastgltf::Filter::Linear: texture->_minification = MINIFICATION_TYPE::MIN_LINEAR; break;
                    case fastgltf::Filter::NearestMipMapNearest: texture->_minification = MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_NEAREST; break;
                    case fastgltf::Filter::LinearMipMapNearest: texture->_minification = MINIFICATION_TYPE::MIN_LINEAR_MIPMAP_NEAREST; break;
                    case fastgltf::Filter::LinearMipMapLinear: texture->_minification = MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_LINEAR; break;
                    default: texture->_minification = MINIFICATION_TYPE::MIN_LINEAR_MIPMAP_LINEAR; break;
                    }
                }

                //switch (model.samplers[t.sampler].wrapR) {
                //case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: texture->_wrappingR = WRAPPING_TYPE::WRAP_CLAMP_TO_EDGE; break;
                //case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: texture->_wrappingR = WRAPPING_TYPE::WRAP_MIRRORED_REPEAT; break;
                //default: texture->_wrappingR = WRAPPING_TYPE::WRAP_REPEAT; break;
                //}

                switch (asset->samplers[t.samplerIndex.value()].wrapS) {
                case fastgltf::Wrap::ClampToEdge: texture->_wrappingS = WRAPPING_TYPE::WRAP_CLAMP_TO_EDGE; break;
                case fastgltf::Wrap::MirroredRepeat: texture->_wrappingS = WRAPPING_TYPE::WRAP_MIRRORED_REPEAT; break;
                default: texture->_wrappingS = WRAPPING_TYPE::WRAP_REPEAT; break;
                }

                switch (asset->samplers[t.samplerIndex.value()].wrapT) {
                case fastgltf::Wrap::ClampToEdge: texture->_wrappingT = WRAPPING_TYPE::WRAP_CLAMP_TO_EDGE; break;
                case fastgltf::Wrap::MirroredRepeat: texture->_wrappingT = WRAPPING_TYPE::WRAP_MIRRORED_REPEAT; break;
                default: texture->_wrappingT = WRAPPING_TYPE::WRAP_REPEAT; break;
                }
            }

            addTexture(texture);
        }
        
        // ------------- MATERIALS
        unsigned int startIdMaterials = (unsigned int)_materials.size();
        _materials.reserve(startIdMaterials + asset->materials.size());
        for (fastgltf::Material m : asset->materials)
            addMaterial(new Material(
                m.pbrData.has_value() ? glm::make_vec4(m.pbrData.value().baseColorFactor.data()) : VEC4F_ONE,
                glm::make_vec3(m.emissiveFactor.data()),
                1.f, // todo find better
                m.pbrData.has_value() ? m.pbrData.value().metallicFactor : 0.f,
                m.pbrData.has_value() ? m.pbrData.value().roughnessFactor : 1.f,
                (m.alphaMode == fastgltf::AlphaMode::Opaque ? 0. : (m.alphaMode == fastgltf::AlphaMode::Blend ? 1. : m.alphaCutoff)),
                m.doubleSided,
                m.pbrData.has_value() ? (m.pbrData.value().baseColorTexture.has_value() ? _textures[startIdTextures + m.pbrData.value().baseColorTexture.value().textureIndex] : nullptr) : nullptr,
                m.pbrData.has_value() ? (m.pbrData.value().metallicRoughnessTexture.has_value() ?_textures[startIdTextures + m.pbrData.value().metallicRoughnessTexture.value().textureIndex] : nullptr) : nullptr,
                m.normalTexture.has_value() ? _textures[startIdTextures + m.normalTexture.value().textureIndex] : nullptr,
                m.occlusionTexture.has_value() ? _textures[startIdTextures + m.occlusionTexture.value().textureIndex] : nullptr,
                m.emissiveTexture.has_value() ? _textures[startIdTextures + m.emissiveTexture.value().textureIndex] : nullptr
            ));

        // ------------- MESHES
        unsigned int startIdMeshes = (unsigned int)_meshes.size();
        _meshes.reserve(startIdMeshes + asset->meshes.size());
        for (fastgltf::Mesh m : asset->meshes) {
            Mesh* newMesh = new Mesh();
            m.primitives.reserve(m.primitives.size());
            for (fastgltf::Primitive p : m.primitives) { // differents type pour la geometry
                if (p.type != fastgltf::PrimitiveType::Triangles) throw std::runtime_error("Fail to load file: primitives must be define by triangles.");
                Primitive* newPrimitive = new Primitive(_materials[(p.materialIndex.has_value() ? startIdMaterials+p.materialIndex.value() : 0)]);
                
                if(!p.attributes.contains("POSITION")) throw std::runtime_error("Fail to load file: primitives must contain POSITION.");
                if (!p.attributes.contains("NORMAL")) throw std::runtime_error("Fail to load file: primitives must contain NORMAL.");
                if (!p.attributes.contains("TEXCOORD_0")) throw std::runtime_error("Fail to load file: primitives must contain TEXCOORD_0.");
                if (!p.indicesAccessor.has_value()) throw std::runtime_error("Fail to load file: primitive indices must be define.");
                
                fastgltf::Accessor a_position = asset->accessors[p.attributes.at("POSITION")];
                fastgltf::Accessor a_normal = asset->accessors[p.attributes.at("NORMAL")];
                fastgltf::Accessor a_texcoord = asset->accessors[p.attributes.at("TEXCOORD_0")];
                fastgltf::Accessor a_indices = asset->accessors[p.indicesAccessor.value()];

                if (!((a_position.count == a_normal.count) && (a_normal.count == a_texcoord.count))) throw std::runtime_error("Fail to load file: primitive vertices must have the same number of position, normal, tangent and texcoord0.");

                if(!a_position.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                if (!a_normal.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                if (!a_texcoord.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                if (!a_indices.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                
                fastgltf::BufferView bv_position = asset->bufferViews[a_position.bufferViewIndex.value()];
                fastgltf::BufferView bv_normal = asset->bufferViews[a_normal.bufferViewIndex.value()];
                fastgltf::BufferView bv_texcoord = asset->bufferViews[a_texcoord.bufferViewIndex.value()];
                
                //const float* positionsBuffer = reinterpret_cast<const float*>(&asset.buffers[bv_position.bufferIndex].data[a_position.byteOffset + bv_position.byteOffset]);
                //const float* normalsBuffer = reinterpret_cast<const float*>(&asset.buffers[bv_normal.bufferIndex].data[a_normal.byteOffset + bv_normal.byteOffset]);
                //const float* texCoord0Buffer = reinterpret_cast<const float*>(&asset.buffers[bv_texcoord.bufferIndex].data[a_texcoord.byteOffset + bv_texcoord.byteOffset]);
                fastgltf::BufferView bv_indices = asset->bufferViews[a_indices.bufferViewIndex.value()];

                newPrimitive->getVertices().reserve(a_position.count);
                for (unsigned int i=0; i<a_position.count ;i++) {
                    Vertex v = Vertex{
                                        ._position = glm::make_vec3(&positionsBuffer[i * 3]),
                                        ._normal = glm::normalize(glm::make_vec3(&normalsBuffer[i * 3])),
                                        ._uv = glm::make_vec2(&texCoordsBuffer[i * 2])
                                     };
                    v._tangent = glm::cross(v._normal, VEC3F_X);
                    if (glm::length(v._tangent) < 0.1) v._tangent = glm::cross(v._normal, VEC3F_Y);
                    v._tangent = glm::normalize(v._tangent);
                    v._bitangent = glm::normalize(glm::cross(v._normal, v._tangent));
                    newPrimitive->addVertex(v) ;
                }

                //enum class ComponentType : std::uint32_t {
                //    Invalid = 0,
                //    Byte = (8 << 16) | 5120,
                //    UnsignedByte = (8 << 16) | 5121,
                //    Short = (16 << 16) | 5122,
                //    UnsignedShort = (16 << 16) | 5123,
                //    Int = (32 << 16) | 5124,
                //    UnsignedInt = (32 << 16) | 5125,
                //    Float = (32 << 16) | 5126,
                //    Double = (64 << 16) | 5130,
                //};

                // data => datasource ! 
                switch (a_indices.componentType) {
                case fastgltf::ComponentType::UnsignedInt:
                    newPrimitive->setIndices(reinterpret_cast<const unsigned int*>(&asset->buffers[bv_indices.bufferIndex].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                    break;
                case fastgltf::ComponentType::UnsignedShort:
                    newPrimitive->setIndices(reinterpret_cast<const unsigned short*>(&asset->buffers[bv_indices.bufferIndex].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                    break;
                case fastgltf::ComponentType::UnsignedByte:
                    newPrimitive->setIndices(reinterpret_cast<const unsigned char*>(&asset->buffers[bv_indices.bufferIndex].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                    break;
                }

                newMesh->addPrimitive(newPrimitive);
            }
            addMesh(newMesh);
        }

        // ------------- LIGHTS
        unsigned int startIdLights = (unsigned int)_lights.size();
        _lights.reserve(startIdLights + asset->lights.size());
        for (fastgltf::Light l : asset->lights) {
            switch (l.type) {
            case fastgltf::LightType::Spot:
                if (l.innerConeAngle.has_value() && l.outerConeAngle.has_value()) {
                    addLight(new Light(LIGHT_TYPE::SPOT, glm::make_vec3(l.color.data()), l.intensity, l.innerConeAngle.value(), l.outerConeAngle.value()));
                    break;
                }
            case fastgltf::LightType::Point:       addLight(new Light(LIGHT_TYPE::POINT, glm::make_vec3(l.color.data()), l.intensity)); break;
            case fastgltf::LightType::Directional: addLight(new Light(LIGHT_TYPE::DIRECTIONAL, glm::make_vec3(l.color.data()), l.intensity)); break;
            }
        }

        // ------------- CAMERAS
        unsigned int startIdCameras = (unsigned int)_cameras.size();
        _cameras.reserve(startIdCameras + asset->cameras.size());
        for (fastgltf::Camera c : asset->cameras) {
            if (std::holds_alternative<fastgltf::Camera::Perspective>(c.camera)) { // CHECK use aspect ratio ??
                fastgltf::Camera::Perspective cam_p = std::get<0>(c.camera);
                addCamera(new Camera(cam_p.yfov, cam_p.aspectRatio.has_value() ? cam_p.aspectRatio.value() : 1.f, cam_p.znear, cam_p.zfar.has_value() ? cam_p.zfar.value() : FLOAT_MAX, CAMERA_TYPE::PERSPECTIVE));
            }
            else if (std::holds_alternative<fastgltf::Camera::Orthographic>(c.camera)) { // CHECK multiply by real aspect ratio ?
                fastgltf::Camera::Orthographic cam_p = std::get<1>(c.camera);
                addCamera(new Camera(cam_p.xmag, cam_p.ymag, cam_p.znear, cam_p.zfar, CAMERA_TYPE::ORTHOGRAPHIC));
            }
        }

        // ------------- SCENE GRAPH
        unsigned int startIdSceneGraph = (unsigned int)_sceneGraph.size();
        _sceneGraph.reserve(startIdSceneGraph + asset->nodes.size());
        for (fastgltf::Scene s : asset->scenes)
            for (int id : s.nodeIndices)
                _createSceneGraph(id, nullptr, startIdMeshes, startIdLights, startIdCameras, asset->nodes);

        if (_cameras.size() > 1 && _cameras[1]->getNumberInstances() > 0) _mainCamera = Vec2i(1, 0);       

        std::cout << "Finished to load " << p_path << std::endl;
    }*/
}
}
