#include "scene_manager.hpp"

#include "core.hpp"
#include "types.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "application.hpp"
#include "renderer/renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <variant>
#include <optional>
#include <iostream>

namespace M3D
{
namespace Scene
{
    SceneManager::SceneManager(const int p_width, const int p_height) {
        addMaterial(Material(VEC4F_ONE, VEC3F_ZERO, 0.f, 0.f, 1.f, 0.f, true, nullptr, nullptr, nullptr, nullptr));
        
        addCamera(Camera(PIf/2.f, 1.f, 1e-2f, 1e3f ,CAMERA_TYPE::PERSPECTIVE));
        addNode(SceneGraphNode(VEC3F_Z, VEC3F_ONE, QUATF_ID));
        addInstance(_cameras[0], &_sceneGraphNodes[0]);
        _mainCamera = Vec2i(0, 0);

        resize(p_width, p_height);
    }

    SceneManager::~SceneManager() { 
        for (int i = 0; i < _meshes.size() ;i++) Application::getInstance().getRenderer().deleteMesh(&_meshes[i]);
        for (int i = 0; i < _textures.size() ;i++) Application::getInstance().getRenderer().deleteTexture(&_textures[i]);
    }

    Camera& SceneManager::getMainCamera() { return _cameras[_mainCamera.x]; }
    SceneGraphNode* SceneManager::getMainCameraSceneGraphNode() { return _cameras[_mainCamera.x].getInstance(_mainCamera.y); }
    const Mat4f SceneManager::getMainCameraTransformation() { return _cameras[_mainCamera.x].getInstance(_mainCamera.y)->getTransformation(); }
    const Mat4f SceneManager::getMainCameraViewMatrix() { return _cameras[_mainCamera.x].getViewMatrix(_mainCamera.y); }
    const Mat4f SceneManager::getMainCameraProjectionMatrix() { return _cameras[_mainCamera.x].getProjectionMatrix(); }

    void SceneManager::loadNewScene(const std::filesystem::path& p_path) { clearScene(); _loadFile(p_path); resize(Application::getInstance().getWidth(), Application::getInstance().getHeight()); }
    void SceneManager::addAsset(const std::filesystem::path& p_path) { _loadFile(p_path); resize(Application::getInstance().getWidth(), Application::getInstance().getHeight()); }

    void SceneManager::addCamera(Camera p_camera) { _cameras.push_back(p_camera); }
    void SceneManager::addLight(Light p_light) { _lights.push_back(p_light); }
    void SceneManager::addMesh(Mesh p_mesh) { _meshes.push_back(p_mesh); Application::getInstance().getRenderer().createMesh(&_meshes[_meshes.size()-1]); }
    void SceneManager::addMaterial(Material p_material) { _materials.push_back(p_material); }
    void SceneManager::addTexture(Texture p_texture) { _textures.push_back(p_texture); Application::getInstance().getRenderer().createTexture(&_textures[_textures.size()-1]); }
    void SceneManager::addImage(Image p_image) { _images.push_back(p_image); }
    void SceneManager::addNode(SceneGraphNode p_node) { _sceneGraphNodes.push_back(p_node); }

    void SceneManager::addInstance(Camera& p_camera, SceneGraphNode* p_node) { p_camera.addInstance(p_node); }
    void SceneManager::addInstance(Light& p_light, SceneGraphNode* p_node) { p_light.addInstance(p_node); }
    void SceneManager::addInstance(Mesh& p_mesh, SceneGraphNode* p_node) {
        p_mesh.addInstance(p_node);

        Application::getInstance().getRenderer().addInstanceMesh(
            &p_mesh,
            p_node->getTransformation(),
            getMainCameraViewMatrix(),
            getMainCameraProjectionMatrix()
        );

        p_node->setDirtyFalse();
    }

    void SceneManager::resize(const int p_width, const int p_height) { for (int i=0; i<_cameras.size() ;i++) _cameras[i].setScreenSize(p_width,p_height); }

    void SceneManager::update(unsigned long long p_deltaTime) {
        SceneGraphNode* cameraInstance = _cameras[_mainCamera.x].getInstance(_mainCamera.y);
        
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

        for (unsigned int i=0; i<_meshes.size() ;i++)
            for (unsigned int j=0; j<_meshes[i].getNumberInstances() ;j++){
                if (_meshes[i].getInstance(j)->isDirty() || cameraInstance->isDirty() || _cameras[_mainCamera.x].isDirty())
                    Application::getInstance().getRenderer().updateInstanceMesh(
                        &_meshes[i], j,
                        _meshes[i].getInstance(j)->getTransformation(),
                        _cameras[_mainCamera.x].getViewMatrix(_mainCamera.y),
                        _cameras[_mainCamera.x].getProjectionMatrix()
                    );
                
                _meshes[i].getInstance(j)->setDirtyFalse();
            }
        cameraInstance->setDirtyFalse();
        _cameras[_mainCamera.x].setDirtyFalse();
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
        for (int i = 0; i<_meshes.size() ;i++) Application::getInstance().getRenderer().deleteMesh(&_meshes[i]);
        for (int i = 0; i<_textures.size() ;i++) Application::getInstance().getRenderer().deleteTexture(&_textures[i]);
        
        _cameras.erase(_cameras.begin()+1,_cameras.end());
        _materials.erase(_materials.begin()+1,_materials.end());
        _sceneGraphNodes.erase(_sceneGraphNodes.begin()+1,_sceneGraphNodes.end());

        _meshes.clear();
        _textures.clear();
        _images.clear();
        _lights.clear();
        
        //_sceneGraph[0]->clearChilds();
        _mainCamera = Vec2i(0, 0);
    }

    void SceneManager::_loadFile(const std::filesystem::path& p_path)
    {
        std::cout << "Start loading " << p_path << std::endl;

        fastgltf::Parser _parser = fastgltf::Parser( fastgltf::Extensions::KHR_lights_punctual | fastgltf::Extensions::KHR_materials_emissive_strength );

        fastgltf::GltfDataBuffer data;
        data.loadFromFile(p_path);

        fastgltf::Expected<fastgltf::Asset> asset = _parser.loadGltfBinary(&data, p_path.parent_path(), fastgltf::Options::DecomposeNodeMatrices | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::DontRequireValidAssetMember);

        // ------------- IMAGES
        unsigned int startIdImages = (unsigned int)_images.size();
        _images.reserve(startIdImages + asset->images.size());
        for (fastgltf::Image& i : asset->images) {
            if (!std::holds_alternative<fastgltf::sources::BufferView>(i.data)) throw std::runtime_error("Image data source invalid!");

            fastgltf::BufferView& bufferView = asset->bufferViews[std::get<1>(i.data).bufferViewIndex];
            fastgltf::Buffer& buffer = asset->buffers[bufferView.bufferIndex];

            if (!std::holds_alternative<fastgltf::sources::Array>(buffer.data)) throw std::runtime_error("Image data source invalid!");

            int width, height, bitDepth, nbChannels;
            unsigned char* data;

            if (stbi_is_16_bit_from_memory(std::get<3>(buffer.data).bytes.data() + bufferView.byteOffset, static_cast<int>(bufferView.byteLength))) {
                bitDepth = 16;
                data = reinterpret_cast<unsigned char*>(stbi_load_16_from_memory(std::get<3>(buffer.data).bytes.data() + bufferView.byteOffset, static_cast<int>(bufferView.byteLength), &width, &height, &nbChannels, 0));
            } else {
                bitDepth = 8;
                data = stbi_load_from_memory(std::get<3>(buffer.data).bytes.data() + bufferView.byteOffset, static_cast<int>(bufferView.byteLength), &width, &height, &nbChannels, 0);
            }

            addImage(Image(width, height, bitDepth, nbChannels, data));
            stbi_image_free(data);
        }
        std::cout << "images loaded: " << _images.size() - startIdImages << std::endl;

        // ------------- TEXTURES
        unsigned int startIdTextures = (unsigned int)_textures.size();
        _textures.reserve(startIdTextures + asset->textures.size());
        for (fastgltf::Texture& t : asset->textures) {
            Texture texture = Texture();

            if (!t.imageIndex.has_value()) throw std::runtime_error("Texture: invalid image index!");
            texture._image = &_images[startIdImages + t.imageIndex.value()];

            if (t.samplerIndex.has_value()) {
                fastgltf::Filter magFilter = fastgltf::Filter::Nearest;
                if (asset->samplers[t.samplerIndex.value()].magFilter.has_value()) magFilter = asset->samplers[t.samplerIndex.value()].magFilter.value();
                texture._magnification = ((magFilter == fastgltf::Filter::Nearest) ? MAGNIFICATION_TYPE::MAG_NEAREST : MAGNIFICATION_TYPE::MAG_LINEAR);
                
                fastgltf::Filter minFilter = fastgltf::Filter::Nearest;
                if (asset->samplers[t.samplerIndex.value()].minFilter.has_value()) minFilter = asset->samplers[t.samplerIndex.value()].minFilter.value();
                switch (minFilter) {
                case fastgltf::Filter::Linear: texture._minification = MINIFICATION_TYPE::MIN_LINEAR; break;
                case fastgltf::Filter::NearestMipMapNearest: texture._minification = MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_NEAREST; break;
                case fastgltf::Filter::LinearMipMapNearest: texture._minification = MINIFICATION_TYPE::MIN_LINEAR_MIPMAP_NEAREST; break;
                case fastgltf::Filter::NearestMipMapLinear: texture._minification = MINIFICATION_TYPE::MIN_NEAREST_MIPMAP_LINEAR; break;
                case fastgltf::Filter::LinearMipMapLinear: texture._minification = MINIFICATION_TYPE::MIN_LINEAR_MIPMAP_LINEAR; break;
                default: texture._minification = MINIFICATION_TYPE::MIN_NEAREST;
                }

                switch (asset->samplers[t.samplerIndex.value()].wrapS) {
                case fastgltf::Wrap::ClampToEdge: texture._wrappingS = WRAPPING_TYPE::CLAMP_TO_EDGE; break;
                case fastgltf::Wrap::MirroredRepeat: texture._wrappingS = WRAPPING_TYPE::MIRRORED_REPEAT; break;
                default: texture._wrappingS = WRAPPING_TYPE::REPEAT; break;
                }

                switch (asset->samplers[t.samplerIndex.value()].wrapT) {
                case fastgltf::Wrap::ClampToEdge: texture._wrappingT = WRAPPING_TYPE::CLAMP_TO_EDGE; break;
                case fastgltf::Wrap::MirroredRepeat: texture._wrappingT = WRAPPING_TYPE::MIRRORED_REPEAT; break;
                default: texture._wrappingT = WRAPPING_TYPE::REPEAT; break;
                }
            }

            addTexture(texture);
        }
        std::cout << "textures loaded: " << _textures.size() - startIdTextures << std::endl;
        
        // ------------- MATERIALS
        unsigned int startIdMaterials = (unsigned int)_materials.size();
        _materials.reserve(startIdMaterials + asset->materials.size());
        for (fastgltf::Material& m : asset->materials) {
            addMaterial(Material(
                glm::make_vec4(m.pbrData.baseColorFactor.data()),
                glm::make_vec3(m.emissiveFactor.data()),
                (float)m.emissiveStrength,
                (float)m.pbrData.metallicFactor,
                (float)m.pbrData.roughnessFactor,
                (m.alphaMode == fastgltf::AlphaMode::Opaque ? 0.f : (m.alphaMode == fastgltf::AlphaMode::Blend ? 1.f : (float)m.alphaCutoff)),
                m.doubleSided,
                (m.pbrData.baseColorTexture.has_value() ? &_textures[startIdTextures + m.pbrData.baseColorTexture.value().textureIndex] : nullptr),
                (m.pbrData.metallicRoughnessTexture.has_value() ? &_textures[startIdTextures + m.pbrData.metallicRoughnessTexture.value().textureIndex] : nullptr),
                (m.normalTexture.has_value() ? &_textures[startIdTextures + m.normalTexture.value().textureIndex] : nullptr),
                (m.emissiveTexture.has_value() ? &_textures[startIdTextures + m.emissiveTexture.value().textureIndex] : nullptr)
            ));
        }
        std::cout << "materials loaded: " << _materials.size() - startIdMaterials << std::endl;

        // ------------- MESHES
        unsigned int startIdMeshes = (unsigned int)_meshes.size();
        _meshes.reserve(startIdMeshes + asset->meshes.size());
        for (fastgltf::Mesh& m : asset->meshes) {
            std::vector<SubMesh> subMeshes;
            subMeshes.reserve(m.primitives.size());
            for (fastgltf::Primitive& p : m.primitives) {
                if (!p.indicesAccessor.has_value()) throw std::runtime_error("Fail to load file: primitive indices must be define.");
                if (p.type != fastgltf::PrimitiveType::Triangles) throw std::runtime_error("Fail to load file: primitive must be define by triangles.");

                auto itPosition = p.findAttribute("POSITION");
                if (itPosition == p.attributes.end()) throw std::runtime_error("Fail to load file: primitive must contain position buffer.");
                fastgltf::Accessor& a_position = asset->accessors[itPosition->second];

                auto itNormal = p.findAttribute("NORMAL");
                if (itNormal == p.attributes.end()) throw std::runtime_error("Fail to load file: primitive must contain normal buffer.");
                fastgltf::Accessor& a_normal = asset->accessors[itNormal->second];

                auto itTexcoord = p.findAttribute("TEXCOORD_0");
                if (itTexcoord == p.attributes.end()) throw std::runtime_error("Fail to load file: primitive must contain texcoord0 buffer.");
                fastgltf::Accessor& a_texcoord = asset->accessors[itTexcoord->second];

                if (!((a_position.count == a_normal.count) && (a_normal.count == a_texcoord.count))) throw std::runtime_error("Fail to load file: primitive vertices must have the same number of position, normal and texcoord0.");
                if (a_texcoord.componentType != fastgltf::ComponentType::Float) throw std::runtime_error("Fail to load file: texcoord0 must be float only.");

                fastgltf::Accessor& a_indices = asset->accessors[p.indicesAccessor.value()];
                if (!a_indices.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: primitive indices must be define.");
                fastgltf::BufferView& bv_indices = asset->bufferViews[a_indices.bufferViewIndex.value()];

                std::vector<unsigned int> indices;
                
                if (!std::holds_alternative<fastgltf::sources::Array>(asset->buffers[bv_indices.bufferIndex].data)) throw std::runtime_error("Primitive indices type must be array!");

                if (a_indices.componentType == fastgltf::ComponentType::UnsignedInt) {
                    const unsigned int* data = reinterpret_cast<const unsigned int*>(std::get<3>(asset->buffers[bv_indices.bufferIndex].data).bytes.data() + a_indices.byteOffset + bv_indices.byteOffset);
                    indices = std::vector<unsigned int>(data, &data[a_indices.count]);
                }
                else if (a_indices.componentType == fastgltf::ComponentType::UnsignedShort) {
                    indices.reserve((unsigned int)a_indices.count);
                    const unsigned short* data = reinterpret_cast<const unsigned short*>(std::get<3>(asset->buffers[bv_indices.bufferIndex].data).bytes.data() + a_indices.byteOffset + bv_indices.byteOffset);
                    for (unsigned int i = 0; i < (unsigned int)a_indices.count; i++) indices.push_back(data[i]);
                }
                else if (a_indices.componentType == fastgltf::ComponentType::UnsignedByte) {
                    indices.reserve((unsigned int)a_indices.count);
                    const unsigned char* data = reinterpret_cast<const unsigned char*>(std::get<3>(asset->buffers[bv_indices.bufferIndex].data).bytes.data() + a_indices.byteOffset + bv_indices.byteOffset);
                    for (unsigned int i = 0; i < (unsigned int)a_indices.count; i++) indices.push_back(data[i]);
                }

                if (!a_position.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: primitive positions must be define.");
                fastgltf::BufferView& bv_position = asset->bufferViews[a_position.bufferViewIndex.value()];
                if (!std::holds_alternative<fastgltf::sources::Array>(asset->buffers[bv_position.bufferIndex].data)) throw std::runtime_error("Fail to load file: primitive positions type must be array!");
                const float* positionsBuffer = reinterpret_cast<const float*>(std::get<3>(asset->buffers[bv_position.bufferIndex].data).bytes.data() + a_position.byteOffset + bv_position.byteOffset);

                if (!a_normal.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: primitive normals must be define.");
                fastgltf::BufferView& bv_normal = asset->bufferViews[a_normal.bufferViewIndex.value()];
                if (!std::holds_alternative<fastgltf::sources::Array>(asset->buffers[bv_normal.bufferIndex].data)) throw std::runtime_error("Fail to load file: primitive normals type must be array!");
                const float* normalsBuffer = reinterpret_cast<const float*>(std::get<3>(asset->buffers[bv_normal.bufferIndex].data).bytes.data() + a_normal.byteOffset + bv_normal.byteOffset);

                if (!a_texcoord.bufferViewIndex.has_value()) throw std::runtime_error("Fail to load file: primitive texcoord0 must be define.");
                fastgltf::BufferView& bv_texcoord = asset->bufferViews[a_texcoord.bufferViewIndex.value()];
                if (!std::holds_alternative<fastgltf::sources::Array>(asset->buffers[bv_normal.bufferIndex].data)) throw std::runtime_error("Fail to load file: primitive texcoord0 type must be array!");
                const float* uvsBuffer = reinterpret_cast<const float*>(std::get<3>(asset->buffers[bv_texcoord.bufferIndex].data).bytes.data() + a_texcoord.byteOffset + bv_texcoord.byteOffset);

                std::vector<Vertex> vertices;
                vertices.reserve(a_position.count);
                for (unsigned int i = 0; i < a_position.count; i++) {
                    Vertex v = Vertex{
                        ._position = glm::make_vec3(&positionsBuffer[i * 3]),
                        ._normal = glm::normalize(glm::make_vec3(&normalsBuffer[i * 3])),
                        ._uv = glm::make_vec2(&uvsBuffer[i * 2])
                    };
                    v._tangent = glm::cross(v._normal, VEC3F_X);
                    if (glm::length(v._tangent) < 0.1) v._tangent = glm::cross(v._normal, VEC3F_Y);
                    v._tangent = glm::normalize(v._tangent);
                    v._bitangent = glm::normalize(glm::cross(v._normal, v._tangent));
                    vertices.push_back(v);
                }
                subMeshes.push_back(SubMesh(&_materials[((p.materialIndex.has_value()) ? startIdMaterials + p.materialIndex.value() : 0)], vertices, indices));
            }
            addMesh(Mesh(subMeshes));
        }
        std::cout << "meshes loaded: " << _meshes.size() - startIdMeshes << std::endl;

        // ------------- LIGHTS
        unsigned int startIdLights = (unsigned int)_lights.size();
        _lights.reserve(startIdLights + asset->lights.size());
        for (fastgltf::Light& l : asset->lights) {
            switch (l.type) {
            case fastgltf::LightType::Spot:
                if (l.innerConeAngle.has_value() && l.outerConeAngle.has_value()) {
                    addLight(Light(LIGHT_TYPE::SPOT, ((l.color.size() != 3) ? VEC3F_ONE : (Vec3f)glm::make_vec3(l.color.data())), (float)l.intensity, (float)l.innerConeAngle.value(), (float)l.outerConeAngle.value()));
                    break;
                }
            case fastgltf::LightType::Point:       addLight(Light(LIGHT_TYPE::POINT, ((l.color.size() != 3) ? VEC3F_ONE : (Vec3f)glm::make_vec3(l.color.data())), (float)l.intensity)); break;
            case fastgltf::LightType::Directional: addLight(Light(LIGHT_TYPE::DIRECTIONAL, ((l.color.size() != 3) ? VEC3F_ONE : (Vec3f)glm::make_vec3(l.color.data())), (float)l.intensity)); break;
            }
        }
        std::cout << "lights loaded: " << _lights.size() - startIdLights << std::endl;

        // ------------- CAMERAS
        unsigned int startIdCameras = (unsigned int)_cameras.size();
        _cameras.reserve(startIdCameras + asset->cameras.size());
        for (fastgltf::Camera& c : asset->cameras) {
            if (std::holds_alternative<fastgltf::Camera::Perspective>(c.camera)) { addCamera(Camera((float)std::get<0>(c.camera).yfov, 1.f, (float)std::get<0>(c.camera).znear, (float)(std::get<0>(c.camera).zfar.has_value() ? std::get<0>(c.camera).zfar.value() : FLOAT_MAX), CAMERA_TYPE::PERSPECTIVE)); }
            else if (std::holds_alternative<fastgltf::Camera::Orthographic>(c.camera)) { addCamera(Camera((float)std::get<1>(c.camera).xmag, (float)std::get<1>(c.camera).ymag, (float)std::get<1>(c.camera).znear, (float)std::get<1>(c.camera).zfar, CAMERA_TYPE::ORTHOGRAPHIC)); }
        }
        std::cout << "cameras loaded: " << _cameras.size() - startIdCameras << std::endl;

        // ------------- SCENE GRAPH
        unsigned int startIdSceneGraph = (unsigned int)_sceneGraphNodes.size();
        _sceneGraphNodes.reserve(startIdSceneGraph + asset->nodes.size());
        for (fastgltf::Node& n : asset->nodes) {
            if (!std::holds_alternative<fastgltf::TRS>(n.transform)) throw std::runtime_error("Transform matrix need to be decompose!");
            addNode(SceneGraphNode(
                (Vec3f)glm::make_vec3(std::get<0>(n.transform).translation.data()),
                (Vec3f)glm::make_vec3(std::get<0>(n.transform).scale.data()),
                Quatf((float)std::get<0>(n.transform).rotation[3], (float)std::get<0>(n.transform).rotation[0], (float)std::get<0>(n.transform).rotation[1], (float)std::get<0>(n.transform).rotation[2])
            ));

            if (n.meshIndex.has_value()) { addInstance(_meshes[startIdMeshes + n.meshIndex.value()], &_sceneGraphNodes[_sceneGraphNodes.size() - 1]); }
            else if (n.cameraIndex.has_value()) { addInstance(_cameras[startIdCameras + n.cameraIndex.value()], &_sceneGraphNodes[_sceneGraphNodes.size() - 1]); }
            else if (n.skinIndex.has_value()) {}
            else if (n.lightIndex.has_value()) { addInstance(_lights[startIdLights + n.lightIndex.value()], &_sceneGraphNodes[_sceneGraphNodes.size() - 1]); }
        }
        for (int i = 0; i < (int)asset->nodes.size(); i++)
            for (size_t id : asset->nodes[i].children)
                _sceneGraphNodes[startIdSceneGraph + i].attach(&_sceneGraphNodes[startIdSceneGraph + id]);
            
        std::cout << "scene graph nodes loaded: " << _sceneGraphNodes.size() - startIdSceneGraph << std::endl;
        
        if (_cameras.size() > 1 && _cameras[1].getNumberInstances() > 0) _mainCamera = Vec2i(1, 0);       

        std::cout << "Finished to load " << p_path << std::endl;
    }
}
}
