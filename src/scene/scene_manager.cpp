#include "scene_manager.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "application.hpp"
#include "renderer/renderer.hpp"

#include "objects/lights/light.hpp"
#include "objects/meshes/mesh.hpp"
#include "objects/meshes/material.hpp"
#include "scene_graph/scene_graph_node.hpp"
#include "objects/cameras/camera.hpp"

//#include <variant>
//#include <optional>
#include <iostream>

namespace M3D
{
namespace Scene
{
    SceneManager::SceneManager(const int p_width, const int p_height) {
        addMaterial(new Material(VEC4F_ONE, VEC3F_ZERO, 0.f, 0.f, 1.f, nullptr, nullptr, nullptr, nullptr, nullptr)); 
        
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
        for (int i=0; i<_lights.size() ;i++) delete _lights[i];
    }

    const Camera* SceneManager::getMainCamera() const { return _cameras[_mainCamera.x]; }
    const SceneGraphNode* SceneManager::getMainCameraSceneGraphNode() const { return _cameras[_mainCamera.x]->getInstance(_mainCamera.y); }
    const Mat4f SceneManager::getMainCameraTransformation() const { return _cameras[_mainCamera.x]->getInstance(_mainCamera.y)->getTransformation(); }
    const Mat4f SceneManager::getMainCameraViewMatrix() const { return _cameras[_mainCamera.x]->getViewMatrix(_mainCamera.y); }
    const Mat4f SceneManager::getMainCameraProjectionMatrix() const { return _cameras[_mainCamera.x]->getProjectionMatrix(); }

    void SceneManager::loadNewScene(const std::string& p_path) { clearScene(); _loadFile(p_path); }

    void SceneManager::addAsset(const std::string& p_path) { _loadFile(p_path); }

    void SceneManager::addCamera(Camera* p_camera) { _cameras.push_back(p_camera); }

    void SceneManager::addLight(Light* p_light) { _lights.push_back(p_light); }

    void SceneManager::addMesh(Mesh* p_mesh) { _meshes.push_back(p_mesh); Application::getInstance().getRenderer().createMesh(p_mesh); }

    void SceneManager::addMaterial(Material* p_material) { _materials.push_back(p_material); }

    void SceneManager::addTexture(Image* p_image) { _textures.push_back(p_image); Application::getInstance().getRenderer().createTexture(p_image); }

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

        for (int i=0; i<_meshes.size() ;i++) { Application::getInstance().getRenderer().deleteMesh(_meshes[i]); delete _meshes[i]; }
        for (int i=0; i<_textures.size() ;i++) { Application::getInstance().getRenderer().deleteTexture(_textures[i]); delete _textures[i]; }
        for (int i=0; i<_lights.size() ;i++) delete _lights[i];
        _meshes.clear();
        _textures.clear();
        _lights.clear();

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

    void SceneManager::_createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, unsigned int p_meshOffset, unsigned int p_lightOffset, unsigned int p_camOffset, tinygltf::Model p_model) {
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

        // ------------- TEXTURES
        unsigned int startIdTextures = (unsigned int)_textures.size();
        _textures.reserve(startIdTextures + model.textures.size());
        for (tinygltf::Texture t : model.textures)
            addTexture(new Image(
                model.images[t.source].width,
                model.images[t.source].height,
                model.images[t.source].component,
                model.images[t.source].bits,
                0,
                model.images[t.source].image
            ));

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
                (m.pbrMetallicRoughness.baseColorTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.pbrMetallicRoughness.baseColorTexture.index],
                (m.pbrMetallicRoughness.metallicRoughnessTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.pbrMetallicRoughness.metallicRoughnessTexture.index],
                (m.normalTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.normalTexture.index],
                (m.occlusionTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.occlusionTexture.index],
                (m.emissiveTexture.index == -1.) ? nullptr : _textures[startIdTextures + m.emissiveTexture.index]
            ));

        // ------------- MESHES
        unsigned int startIdMeshes = (unsigned int)_meshes.size();
        _meshes.reserve(startIdMeshes + model.meshes.size());
        for (tinygltf::Mesh m : model.meshes) {
            Mesh* newMesh = new Mesh();
            newMesh->getPrimitives().reserve(m.primitives.size());
            for (tinygltf::Primitive p : m.primitives) {
                if (p.indices == -1) throw std::runtime_error("Fail to load file: primitive indices must be define.");

                Primitive* newPrimitive = new Primitive(_materials[startIdMaterials + p.material]);

                bool isTangent = p.attributes.contains("TANGENT");
                tinygltf::Accessor a_position = model.accessors[p.attributes.at("POSITION")];
                tinygltf::Accessor a_normal = model.accessors[p.attributes.at("NORMAL")];
                tinygltf::Accessor a_texcoord = model.accessors[p.attributes.at("TEXCOORD_0")];
                tinygltf::Accessor a_tangent;
                if (isTangent) a_tangent = model.accessors[p.attributes.at("TANGENT")];

                if (!((a_position.count == a_normal.count) && (a_normal.count == a_texcoord.count) && (!isTangent || (a_texcoord.count == a_tangent.count)))) throw std::runtime_error("Fail to load file: primitive vertices must have the same number of position, normal, tangent and texcoord0.");

                tinygltf::BufferView bv_position = model.bufferViews[a_position.bufferView];
                const float* positionsBuffer = reinterpret_cast<const float*>(&model.buffers[bv_position.buffer].data[a_position.byteOffset + bv_position.byteOffset]);

                tinygltf::BufferView bv_normal = model.bufferViews[a_normal.bufferView];
                const float* normalsBuffer = reinterpret_cast<const float*>(&model.buffers[bv_normal.buffer].data[a_normal.byteOffset + bv_normal.byteOffset]);

                const float* tangentsBuffer = nullptr;
                if (isTangent) {
                    tinygltf::BufferView bv_tangent = model.bufferViews[a_tangent.bufferView];
                    tangentsBuffer = reinterpret_cast<const float*>(&model.buffers[bv_tangent.buffer].data[a_tangent.byteOffset + bv_tangent.byteOffset]);
                }

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
                    v._tangent = (isTangent) ? glm::normalize(glm::make_vec3(&tangentsBuffer[i * 3])) : VEC3F_X; // TODO implement better
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

        // ------------- CAMERAS
        int idStartCameras = (int)_cameras.size();
        _cameras.reserve(idStartCameras + (int)model.cameras.size());
        for (tinygltf::Camera c : model.cameras) {
            if (c.type == "perspective") {  // CHECK use aspect ratio ??
                addCamera(new Camera((float)c.perspective.yfov, float(Application::getInstance().getWidth())/float(Application::getInstance().getHeight()), (float)c.perspective.znear, (float)c.perspective.zfar, CAMERA_TYPE::PERSPECTIVE));
            }
            else if (c.type == "orthographic") { // CHECK multiply by real aspect ratio ?
                addCamera(new Camera((float)c.orthographic.xmag, (float)c.orthographic.ymag, (float)c.orthographic.znear, (float)c.orthographic.zfar, CAMERA_TYPE::ORTHOGRAPHIC));
            }
        }
        
        // ------------- SCENE GRAPH
        unsigned int startIdSceneGraph = (unsigned int)_sceneGraph.size();
        _sceneGraph.reserve(startIdSceneGraph + model.nodes.size());
        for (tinygltf::Scene s : model.scenes)
            for (int id : s.nodes)
                _createSceneGraph(id, nullptr, startIdMeshes, startIdLights, idStartCameras, model);

        // ============================= TODO virer
        if (_lights.size() == 0) {
            SceneGraphNode* node = new SceneGraphNode(nullptr, Vec3f(0., 0., 0.), Vec3f(1., 1., 1.), Quatf(0.378f, -0.444f, 0.805f, 0.101f));
            Light* light = new Light(LIGHT_TYPE::DIRECTIONAL, Vec4f(1., 1., 1., 1.), 1.);
            addLight(light);
            addNode(node);
            addInstance(light, node);
        }

        if (_cameras.size() > 1 && _cameras[1]->getNumberInstances() > 0) _mainCamera = Vec2i(1, 0);
        // =============================

        std::cout << "Finished to load " << p_path << std::endl;
    }

    /*void SceneManager::_createSceneGraph(int p_idCurrent, SceneGraphNode* p_parent, unsigned int p_meshOffset, unsigned int p_lightOffset, unsigned int p_camOffset, std::vector<fastgltf::Node>& p_nodes) {
        Vec3f translation = (p_nodes[p_idCurrent].translation.size() == 3) ? (Vec3f)glm::make_vec3(p_nodes[p_idCurrent].translation.data()) : VEC3F_ZERO;
        Vec3f scale       = (p_nodes[p_idCurrent].scale.size()       == 3) ? (Vec3f)glm::make_vec3(p_nodes[p_idCurrent].scale.data()) : VEC3F_ONE;
        Quatf rotation    = (p_nodes[p_idCurrent].rotation.size()    == 4) ? Quatf((float)p_nodes[p_idCurrent].rotation[3], (float)p_nodes[p_idCurrent].rotation[0], (float)p_nodes[p_idCurrent].rotation[1], (float)p_nodes[p_idCurrent].rotation[2]) : QUATF_ID;
        SceneGraphNode* current = new SceneGraphNode(p_parent, translation, scale, rotation);
        addNode(current);

        if (p_nodes[p_idCurrent].mesh != -1)        { addInstance(_meshes[p_meshOffset + p_nodes[p_idCurrent].mesh],current); }
        else if (p_nodes[p_idCurrent].camera != -1) { addInstance(_cameras[p_camOffset + p_nodes[p_idCurrent].camera],current); }
        else                                        { addInstance(_lights[p_lightOffset + p_nodes[p_idCurrent].extensions.at("KHR_lights_punctual").Get("light").GetNumberAsInt()],current); }

        for (int id : p_nodes[p_idCurrent].children)
            _createSceneGraph(id, current, p_meshOffset, p_lightOffset, p_camOffset, p_nodes);
    }

    void SceneManager::_loadFile(const std::filesystem::path &p_path)
    {
        std::cout << "Start loading " << p_path << std::endl;

        fastgltf::GltfDataBuffer data;
        data.loadFromFile(p_path);

        std::cout << "load file!" << std::endl;

        std::unique_ptr<fastgltf::glTF> gltf =
            (p_path.extension() == ".gltf") ? // test option => DontRequireValidAssetMember || LoadGLBBuffers || LoadExternalBuffers
            parser.loadGLTF(&data, p_path.parent_path(), fastgltf::Options::DecomposeNodeMatrices ) :
            parser.loadBinaryGLTF(&data, p_path.parent_path(), fastgltf::Options::DecomposeNodeMatrices );

        if (parser.getError() != fastgltf::Error::None) throw std::runtime_error("Fail to load file: " + p_path.string());
        if (gltf->parse() != fastgltf::Error::None) throw std::runtime_error("Fail to parse file: " + p_path.string());    // on peut cibler le parse (-skins/-animations)

        std::unique_ptr<fastgltf::Asset> asset = gltf->getParsedAsset();

        int idStartTextures = (int)_textures.size();
        _textures.reserve(idStartTextures + (int)asset->textures.size());
        for (fastgltf::Texture t : asset->textures) { // care differents samplers + duplicate images
            if(!t.imageIndex.has_value()) throw std::runtime_error("Image: invalid index!"); // change for better
           
            fastgltf::DataSource dataSource = asset->images[t.imageIndex.value()].data;
            if(std::holds_alternative<std::monostate>(dataSource) || std::holds_alternative<fastgltf::sources::CustomBuffer>(dataSource)) throw std::runtime_error("Image data source invalid!"); // change for better
            if(std::holds_alternative<fastgltf::sources::BufferView>(dataSource)) { } //addTexture(new Image()); parse data
            if(std::holds_alternative<fastgltf::sources::FilePath>(dataSource)) { } //addTexture(new Image()); use stb to parse 
            if(std::holds_alternative<fastgltf::sources::Vector>(dataSource)) { } //addTexture(new Image()); memcpy ou use pointer + mime type for datas
        }
        
        int idStartMaterials = (int)_materials.size();
        _materials.reserve(1 + idStartMaterials + (int)asset->materials.size());
        for (fastgltf::Material m : asset->materials)
            addMaterial(new Material(
                m.pbrData.has_value() ? glm::make_vec4(m.pbrData.value().baseColorFactor.data()) : VEC4F_ONE,
                glm::make_vec3(m.emissiveFactor.data()),
                m.pbrData.has_value() ? m.pbrData.value().metallicFactor : 0.f,
                m.pbrData.has_value() ? m.pbrData.value().roughnessFactor : 1.f,
                m.pbrData.has_value() ? (m.pbrData.value().baseColorTexture.has_value() ? _textures[idStartTextures + m.pbrData.value().baseColorTexture.value().textureIndex] : nullptr) : nullptr,
                m.pbrData.has_value() ? (m.pbrData.value().metallicRoughnessTexture.has_value() ?_textures[idStartTextures + m.pbrData.value().metallicRoughnessTexture.value().textureIndex] : nullptr) : nullptr,
                m.normalTexture.has_value() ? _textures[idStartTextures + m.normalTexture.value().textureIndex] : nullptr,
                m.occlusionTexture.has_value() ? _textures[idStartTextures + m.occlusionTexture.value().textureIndex] : nullptr,
                m.emissiveTexture.has_value() ? _textures[idStartTextures + m.emissiveTexture.value().textureIndex] : nullptr
            ));

        int idStartMeshes = (int)_meshes.size();
        _meshes.reserve(idStartMeshes + (int)asset->meshes.size());
        for (fastgltf::Mesh m : asset->meshes) {
            Mesh* newMesh = new Mesh();
            m.primitives.reserve(m.primitives.size());
            for (fastgltf::Primitive p : m.primitives) { // differents type pour la geometry
                if (p.type != fastgltf::PrimitiveType::Triangles) throw std::runtime_error("Fail to load file: primitives must be define by triangles.");
                Primitive* newPrimitive = new Primitive(_materials[(p.materialIndex.has_value() ? idStartMaterials+p.materialIndex.value() : 0)]);
                
                if(!p.attributes.contains("POSITION")) throw std::runtime_error("Fail to load file: primitives must contain POSITION.");
                fastgltf::Accessor a_position = asset->accessors[p.attributes.at("POSITION")];

                if (!p.attributes.contains("NORMAL")) throw std::runtime_error("Fail to load file: primitives must contain NORMAL.");
                fastgltf::Accessor a_normal = asset->accessors[p.attributes.at("NORMAL")];

                if (!p.attributes.contains("TEXCOORD_0")) throw std::runtime_error("Fail to load file: primitives must contain TEXCOORD_0.");
                fastgltf::Accessor a_texcoord = asset->accessors[p.attributes.at("TEXCOORD_0")];
                
                bool isTangent = p.attributes.contains("TANGENT");
                fastgltf::Accessor a_tangent;
                if (isTangent) a_tangent = asset->accessors[p.attributes.at("TANGENT")];

                if (!p.indicesAccessor.has_value()) throw std::runtime_error("Fail to load file: primitive indices must be define.");
                fastgltf::Accessor a_indices = asset->accessors[p.indicesAccessor.value()];

                if (!((a_position.count == a_normal.count) && (a_normal.count == a_texcoord.count) && (!isTangent || (a_texcoord.count == a_tangent.count)))) throw std::runtime_error("Fail to load file: primitive vertices must have the same number of position, normal, tangent and texcoord0.");

                if(!a_position.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                fastgltf::BufferView bv_position = asset->bufferViews[a_position.bufferViewIndex.value()];
                //const float* positionsBuffer = reinterpret_cast<const float*>(&asset.buffers[bv_position.bufferIndex].data[a_position.byteOffset + bv_position.byteOffset]);
                
                if (!a_normal.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                fastgltf::BufferView bv_normal = asset->bufferViews[a_normal.bufferViewIndex.value()];
                //const float* normalsBuffer = reinterpret_cast<const float*>(&asset.buffers[bv_normal.bufferIndex].data[a_normal.byteOffset + bv_normal.byteOffset]);

                if (!a_texcoord.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                fastgltf::BufferView bv_texcoord = asset->bufferViews[a_texcoord.bufferViewIndex.value()];
                //const float* texCoord0Buffer = reinterpret_cast<const float*>(&asset.buffers[bv_texcoord.bufferIndex].data[a_texcoord.byteOffset + bv_texcoord.byteOffset]);

                //const float* tangentsBuffer = nullptr;
                if (isTangent) {
                    if (!a_tangent.bufferViewIndex.has_value()) isTangent = false;
                    else {
                        fastgltf::BufferView bv_tangent = asset->bufferViews[a_tangent.bufferViewIndex.value()];
                        //tangentsBuffer = reinterpret_cast<const float*>(&asset.buffers[bv_tangent.bufferIndex].data[a_tangent.byteOffset + bv_tangent.byteOffset]);
                    }
                }

                if (!a_indices.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: accessor need to have a view buffer index.");
                fastgltf::BufferView bv_indices = asset->bufferViews[a_indices.bufferViewIndex.value()];

                newPrimitive->getVertices().reserve(a_position.count);
                for (unsigned int i=0; i<a_position.count ;i++) {*/
                    /*Vertex v = Vertex{
                                        ._position = glm::make_vec3(&positionsBuffer[i * 3]),
                                        ._normal = glm::normalize(glm::make_vec3(&normalsBuffer[i * 3])),
                                        ._uv = glm::make_vec2(&texCoord0Buffer[i * 2])
                                     };
                    v._tangent = (isTangent) ? glm::normalize(glm::make_vec3(&tangentsBuffer[i * 3])) : VEC3F_X;
                    v._bitangent = glm::normalize(glm::cross(v._normal, v._tangent));
                    newPrimitive->addVertex(v) ;*/
                //}

                /*enum class ComponentType : std::uint32_t {
                    Invalid = 0,
                    Byte = (8 << 16) | 5120,
                    UnsignedByte = (8 << 16) | 5121,
                    Short = (16 << 16) | 5122,
                    UnsignedShort = (16 << 16) | 5123,
                    Int = (32 << 16) | 5124,
                    UnsignedInt = (32 << 16) | 5125,
                    Float = (32 << 16) | 5126,
                    Double = (64 << 16) | 5130,
                };*/

                /*switch (a_indices.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                        newPrimitive->setIndices(reinterpret_cast<const unsigned int*>(&model.buffers[bv_indices.buffer].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                        break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                        newPrimitive->setIndices(reinterpret_cast<const unsigned short*>(&model.buffers[bv_indices.buffer].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                        break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                        newPrimitive->setIndices(reinterpret_cast<const unsigned char*>(&model.buffers[bv_indices.buffer].data[a_indices.byteOffset + bv_indices.byteOffset]), (unsigned int)a_indices.count);
                        break;
                }*/

                /*newMesh->addPrimitive(newPrimitive);
            }
            addMesh(newMesh);
        }

        int idStartLights = (int)_lights.size();
        _lights.reserve(idStartLights + (int)asset->lights.size());
        for (fastgltf::Light l : asset->lights) {
            switch (l.type) {
                case fastgltf::LightType::Spot:
                    if (l.innerConeAngle.has_value() && l.outerConeAngle.has_value()) {
                        addLight(new Light( LIGHT_TYPE::SPOT, glm::make_vec3(l.color.data()), l.intensity, l.innerConeAngle.value(), l.outerConeAngle.value()));
                        break;
                    }
                case fastgltf::LightType::Point: addLight(new Light(LIGHT_TYPE::POINT, glm::make_vec3(l.color.data()), l.intensity)); break;
                case fastgltf::LightType::Directional: addLight(new Light(LIGHT_TYPE::DIRECTIONAL, glm::make_vec3(l.color.data()), l.intensity)); break;
            }
        }

        int idStartCameras = (int)_cameras.size();
        _cameras.reserve(1 + idStartCameras + (int)asset->cameras.size());
        for (fastgltf::Camera c : asset->cameras)       // implement ortho ??
            if (std::holds_alternative<fastgltf::Camera::Perspective>(c.camera)) {  // use aspect ratio ??
                fastgltf::Camera::Perspective cam_p = std::get<0>(c.camera);
                addCamera(new Camera(cam_p.znear, cam_p.zfar.has_value() ? cam_p.zfar.value() : FLOAT_MAX, cam_p.yfov));
            }
        // _mainCamera = 0;
       
        int idStartSceneGraph = (int)_sceneGraph.size();
        _sceneGraph.reserve(idStartSceneGraph + (int)asset->nodes.size());
        for (fastgltf::Scene s : asset->scenes)
            for (int id : s.nodeIndices)
                _createSceneGraph(id, nullptr, idStartMeshes, idStartCameras, idStartLights, asset->nodes);

        std::cout << "Finished to load " << p_path << std::endl;
    }*/
}
}
