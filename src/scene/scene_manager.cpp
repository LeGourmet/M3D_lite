#include "scene_manager.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include "application.hpp"
#include "renderer/renderer.hpp"

namespace M3D
{
namespace Scene
{
    SceneManager::SceneManager() { }       
    SceneManager::~SceneManager() { clearScene(); }

    void SceneManager::addMeshes(const std::string& p_path) { _loadFile(p_path); }

    void SceneManager::addLight(Light* p_light) { _lights.push_back(p_light); }

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

    void SceneManager::removeMesh(MeshTriangle* const p_mesh) {
        std::vector<MeshTriangle*>::iterator it = std::find(_meshes.begin(), _meshes.end(), p_mesh);
        delete _meshes[std::distance(_meshes.begin(), it)];
        _meshes.erase(it);
    }

    void SceneManager::removeLight(Light* const p_light) {
        std::vector<Light*>::iterator it = std::find(_lights.begin(), _lights.end(), p_light);
        delete _lights[std::distance(_lights.begin(), it)];
        _lights.erase(it);
    }

    void SceneManager::clearScene() {
        for (int i=0; i<_meshes.size() ;i++) delete _meshes[i];
        for (int i=0; i<_lights.size() ;i++) delete _lights[i];
        _lights.clear();
        _meshes.clear();
    }

    void SceneManager::_loadMaterial( const std::string& p_path, MeshTriangle* p_meshTri, const aiMaterial* p_mtl)
    {
        aiString texturePath;
        aiColor3D aiCol; 
        float aiS;

        if (p_mtl->GetTextureCount(aiTextureType_AMBIENT) > 0)
        {
            p_mtl->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);
            p_meshTri->_ambientMapPath = p_path + texturePath.C_Str();
            p_meshTri->_hasAmbientMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            p_mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
            p_meshTri->_albedoMapPath = p_path + texturePath.C_Str();
            p_meshTri->_hasAlbedoMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            p_mtl->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
            p_meshTri->_metalnessMapPath = p_path + texturePath.C_Str();
            p_meshTri->_hasMetalnessMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            p_mtl->GetTexture(aiTextureType_SHININESS, 0, &texturePath);
            p_meshTri->_roughnessMapPath = p_path + texturePath.C_Str();
            p_meshTri->_hasRoughnessMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            p_mtl->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
            p_meshTri->_normalMapPath = p_path + texturePath.C_Str();
            p_meshTri->_hasNormalMap = true;
        }

        if (p_mtl->Get(AI_MATKEY_COLOR_AMBIENT , aiCol) == AI_SUCCESS) p_meshTri->_ambient = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (p_mtl->Get(AI_MATKEY_COLOR_DIFFUSE , aiCol) == AI_SUCCESS) p_meshTri->_albedo = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        //if (p_mtl->Get(AI_MATKEY_COLOR_SPECULAR, aiCol) == AI_SUCCESS) p_meshTri->_ks = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (p_mtl->Get(AI_MATKEY_SHININESS     , aiS  ) == AI_SUCCESS) p_meshTri->_roughness = (float)glm::sqrt(2./(2.+p_meshTri->_roughness));
    }

    MeshTriangle* SceneManager::_loadMesh(const aiMesh *const p_mesh)
    {
        MeshTriangle *triMesh = new MeshTriangle();
        triMesh->_hasUVs = p_mesh->HasTextureCoords(0);

        triMesh->getVertices().reserve(p_mesh->mNumVertices);
        for (unsigned int v = 0; v < p_mesh->mNumVertices; ++v)
        {
            Vertex vertex;
            vertex._position = Vec3f(p_mesh->mVertices[v].x, p_mesh->mVertices[v].y, p_mesh->mVertices[v].z);
            vertex._normal = Vec3f(p_mesh->mNormals[v].x, p_mesh->mNormals[v].y, p_mesh->mNormals[v].z);
            if (triMesh->_hasUVs)
            {
                vertex._uv = Vec2f(p_mesh->mTextureCoords[0][v].x, p_mesh->mTextureCoords[0][v].y);
                vertex._tangent = Vec3f(p_mesh->mTangents[v].x, p_mesh->mTangents[v].y, p_mesh->mTangents[v].z);
                vertex._bitangent = Vec3f(p_mesh->mBitangents[v].x, p_mesh->mBitangents[v].y, p_mesh->mBitangents[v].z);
            }
            triMesh->addVertex(vertex);
        }

        triMesh->getIndices().reserve(p_mesh->mNumFaces*3);
        for (unsigned int f = 0; f < p_mesh->mNumFaces; ++f)
        {
            const aiFace &face = p_mesh->mFaces[f];
            triMesh->addTriangle(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
        }

        return triMesh;
    }

    void SceneManager::_loadFile(const std::string &p_path)
    {
        Assimp::Importer importer;

        const aiScene *const scene = importer.ReadFile(p_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
        // aiProcessPreset_TargetRealtime_Fast add =>  aiProcess_SortByPType + maria add | aiProcess_FlipUVs
        if (scene == nullptr) throw std::runtime_error("Fail to load file: " + p_path);

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m) 
        {
            const aiMesh *const mesh = scene->mMeshes[m];
            if (mesh == nullptr) throw std::runtime_error("Fail to load file: " + p_path + ": " + importer.GetErrorString());

            MeshTriangle* triMesh  = _loadMesh(mesh);
            
            const aiMaterial* const mtl = scene->mMaterials[mesh->mMaterialIndex];
            if (mtl != nullptr) _loadMaterial(std::filesystem::absolute(p_path).remove_filename().string(), triMesh, mtl);

            Application::getInstance().getRenderer().createMesh(triMesh);

            _meshes.push_back(triMesh);
        }
    }
}
}
