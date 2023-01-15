#include "scene_manager.hpp"

#include "application.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include <iostream>

namespace M3D
{
namespace Scene
{

    void SceneManager::_loadMaterial( const std::string& p_path, MeshTriangle* p_meshTri, const aiMaterial* p_mtl)
    {
        aiString texturePath;
        aiColor3D aiCol; 
        float aiS = 0.f;

        if (p_mtl->GetTextureCount(aiTextureType_AMBIENT) > 0)
        {
            p_mtl->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);
            p_meshTri->setAmbientMap(p_path + texturePath.C_Str());
            p_meshTri->_hasAmbientMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            p_mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
            p_meshTri->setDiffuseMap(p_path + texturePath.C_Str());
            p_meshTri->_hasDiffuseMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            p_mtl->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
            p_meshTri->setSpecularMap(p_path + texturePath.C_Str());
            p_meshTri->_hasSpecularMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            p_mtl->GetTexture(aiTextureType_SHININESS, 0, &texturePath);
            p_meshTri->setShininessMap(p_path + texturePath.C_Str());
            p_meshTri->_hasShininessMap = true;
        }

        if (p_mtl->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            p_mtl->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
            p_meshTri->setNormalMap(p_path + texturePath.C_Str());
            p_meshTri->_hasNormalMap = true;
        }

        if (p_mtl->Get(AI_MATKEY_COLOR_AMBIENT , aiCol) == AI_SUCCESS) p_meshTri->_ka = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (p_mtl->Get(AI_MATKEY_COLOR_DIFFUSE , aiCol) == AI_SUCCESS) p_meshTri->_kd = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (p_mtl->Get(AI_MATKEY_COLOR_SPECULAR, aiCol) == AI_SUCCESS) p_meshTri->_ks = Vec3f(aiCol.r, aiCol.g, aiCol.b);
        if (p_mtl->Get(AI_MATKEY_SHININESS     , aiS  ) == AI_SUCCESS) p_meshTri->_s  = aiS;
    }

    MeshTriangle* SceneManager::_loadMesh(const aiMesh *const p_mesh)
    {
        MeshTriangle *triMesh = new MeshTriangle(std::string(p_mesh->mName.C_Str()));
        triMesh->_hasUVs = p_mesh->HasTextureCoords(0);

        for (unsigned int v = 0; v < p_mesh->mNumVertices; ++v)
        {
            triMesh->addVertex(p_mesh->mVertices[v].x, p_mesh->mVertices[v].y, p_mesh->mVertices[v].z);
            triMesh->addNormal(p_mesh->mNormals[v].x, p_mesh->mNormals[v].y, p_mesh->mNormals[v].z);
            if (triMesh->_hasUVs)
            {
                triMesh->addUV(p_mesh->mTextureCoords[0][v].x, p_mesh->mTextureCoords[0][v].y);
                triMesh->addTangent(p_mesh->mTangents[v].x, p_mesh->mTangents[v].y, p_mesh->mTangents[v].z);
                triMesh->addBitangent(p_mesh->mBitangents[v].x, p_mesh->mBitangents[v].y, p_mesh->mBitangents[v].z);
            }
        }

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

        const aiScene *const scene = importer.ReadFile(p_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);
        if (scene == nullptr) throw std::runtime_error("Fail to load file: " + p_path);

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m) 
        {
            const aiMesh *const mesh = scene->mMeshes[m];
            if (mesh == nullptr) throw std::runtime_error("Fail to load file: " + p_path + ": " + importer.GetErrorString());
            

            MeshTriangle* triMesh  = _loadMesh(mesh);
            
            const aiMaterial* const mtl = scene->mMaterials[mesh->mMaterialIndex];
            if (mtl != nullptr) _loadMaterial(p_path,triMesh, mtl);

            _meshes.push_back(triMesh);
        }
    }

}
}
