#include "scene.hpp"

#include "application.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <iostream>

namespace M3D
{
namespace Scene
{

    void Scene::_loadMaterial( const std::string& path, MeshTriangle* triMesh, const aiMaterial* mtl)
    {
        aiString texturePath;
        aiColor3D aiCol; 
        float aiS = 0.f;

        if (mtl->GetTextureCount(aiTextureType_AMBIENT) > 0)
        {
            mtl->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);
            triMesh->setAmbientMap(path + texturePath.C_Str());
            triMesh->_hasAmbientMap = true;
        }

        if (mtl->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
            triMesh->setDiffuseMap(path + texturePath.C_Str());
            triMesh->_hasDiffuseMap = true;
        }

        if (mtl->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            mtl->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
            triMesh->setSpecularMap(path + texturePath.C_Str());
            triMesh->_hasSpecularMap = true;
        }

        if (mtl->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            mtl->GetTexture(aiTextureType_SHININESS, 0, &texturePath);
            triMesh->setShininessMap(path + texturePath.C_Str());
            triMesh->_hasShininessMap = true;
        }

        if (mtl->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            mtl->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
            triMesh->setNormalMap(path + texturePath.C_Str());
            triMesh->_hasNormalMap = true;
        }

        if (mtl->Get(AI_MATKEY_COLOR_AMBIENT , aiCol) == AI_SUCCESS) triMesh->_ka = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (mtl->Get(AI_MATKEY_COLOR_DIFFUSE , aiCol) == AI_SUCCESS) triMesh->_kd = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (mtl->Get(AI_MATKEY_COLOR_SPECULAR, aiCol) == AI_SUCCESS) triMesh->_ks = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (mtl->Get(AI_MATKEY_SHININESS     , aiS  ) == AI_SUCCESS) triMesh->_s  = aiS;
    }

    MeshTriangle* Scene::_loadMesh(const aiMesh *const mesh)
    {
        MeshTriangle *triMesh = new MeshTriangle(std::string(mesh->mName.C_Str()));
        triMesh->_hasUVs = mesh->HasTextureCoords(0);

        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            triMesh->addVertex(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            triMesh->addNormal(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
            if (triMesh->_hasUVs)
            {
                triMesh->addUV(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
                triMesh->addTangent(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
                triMesh->addBitangent(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
            }
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            const aiFace &face = mesh->mFaces[f];
            triMesh->addTriangle(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
        }

        return triMesh;
    }

    void Scene::_loadFile(const std::string &p_path)
    {
        Assimp::Importer importer;

        const aiScene *const scene = importer.ReadFile(p_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);
        if (scene == nullptr) throw std::runtime_error("Fail to load file: " + p_path);

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m) 
        {
            const aiMesh *const mesh = scene->mMeshes[m];
            if (mesh == nullptr) throw std::runtime_error("Fail to load file: " + p_path + ": " + importer.GetErrorString());
            

            MeshTriangle* triMesh  = _loadMesh(mesh);
            
            const aiMaterial* const mtl = scene->mMaterials[mesh->mMaterialIndex];
            if (mtl != nullptr) _loadMaterial(triMesh, mtl);

            _meshes.push_back(triMesh);
        }
    }

} // namespace Model
} // namespace TutoVulkan
