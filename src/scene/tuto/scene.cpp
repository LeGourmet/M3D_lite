#include "scene.hpp"

#include "application.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <iostream>

namespace TutoVulkan
{
namespace Model
{

    void Scene::_loadMaterial(MeshTriangle *triMesh, const aiMaterial *mtl)
    {
        std::string path = triMesh->_path;
        aiString texturePath;
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

        if (mtl->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            mtl->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
            triMesh->setNormalMap(path + texturePath.C_Str());
            triMesh->_hasNormalMap = true;
        }

        if (mtl->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            mtl->GetTexture(aiTextureType_SHININESS, 0, &texturePath);
            triMesh->setShininessMap(path + texturePath.C_Str());
            triMesh->_hasShininessMap = true;
        }

        Vec3f ka = VEC3F_ZERO;
        Vec3f kd = Vec3f(1.f);
        Vec3f ks = Vec3f(1.f);
        float s = 64.f;

        if (aiColor3D aiKa; mtl->Get(AI_MATKEY_COLOR_AMBIENT, aiKa) != AI_SUCCESS) ka = Vec3f(aiKa.r, aiKa.g, aiKa.b);
        if (aiColor3D aiKd; mtl->Get(AI_MATKEY_COLOR_DIFFUSE, aiKd) == AI_SUCCESS) kd = Vec3f(aiKd.r, aiKd.g, aiKd.b);
        if (aiColor3D aiKs; mtl->Get(AI_MATKEY_COLOR_SPECULAR, aiKs) == AI_SUCCESS) ks = Vec3f(aiKs.r, aiKs.g, aiKs.b);
        if (float aiS = 0.f; mtl->Get(AI_MATKEY_SHININESS, aiS) == AI_SUCCESS) s = aiS;

        triMesh->_ka = ka;
        triMesh->_kd = kd;
        triMesh->_ks = ks;
        triMesh->_s = s;
    }

    void Scene::_loadMesh(const std::string &p_path, const aiScene *const scene, const aiMesh *const aiMesh, const std::string &meshName)
    {
        MeshTriangle *triMesh = new MeshTriangle(meshName);
        triMesh->_hasUVs = aiMesh->HasTextureCoords(0);

        for (unsigned int v = 0; v < aiMesh->mNumVertices; ++v)
        {
            triMesh->addVertex(aiMesh->mVertices[v].x, aiMesh->mVertices[v].y, aiMesh->mVertices[v].z);
            triMesh->addNormal(aiMesh->mNormals[v].x, aiMesh->mNormals[v].y, aiMesh->mNormals[v].z);
            if (triMesh->_hasUVs)
            {
                triMesh->addUV(aiMesh->mTextureCoords[0][v].x, aiMesh->mTextureCoords[0][v].y);
                triMesh->addTangent(aiMesh->mTangents[v].x, aiMesh->mTangents[v].y, aiMesh->mTangents[v].z);
                triMesh->addBitangent(aiMesh->mBitangents[v].x, aiMesh->mBitangents[v].y, aiMesh->mBitangents[v].z);
            }
        }

        for (unsigned int f = 0; f < aiMesh->mNumFaces; ++f)
        {
            const aiFace &face = aiMesh->mFaces[f];
            triMesh->addTriangle(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
        }

        const aiMaterial *const mtl = scene->mMaterials[aiMesh->mMaterialIndex];
        if (mtl == nullptr)
            std::cerr << "Material undefined," << meshName << " assigned to default material" << std::endl;
        else
            _loadMaterial(triMesh, mtl);

        _meshes.push_back(triMesh);
    }

    void Scene::_loadFile(const std::string &p_path)
    {
        std::cout << "Loading: " << p_path << std::endl;
        Assimp::Importer importer;

        // const unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs;
        const aiScene *const scene = importer.ReadFile(p_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

        if (scene == nullptr) { throw std::runtime_error("Fail to load file: " + p_path); }

        unsigned int cptTriangles = 0;
        unsigned int cptVertices = 0;

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh *const mesh = scene->mMeshes[m];
            if (mesh == nullptr) { throw std::runtime_error("Fail to load file: " + p_path + ": " + importer.GetErrorString()); }

            const std::string meshName = std::string(mesh->mName.C_Str());
            std::cout << "-- Load mesh " << m + 1 << "/" << scene->mNumMeshes << ": " << meshName << std::endl;

            cptTriangles += aiMesh->mNumFaces;
            cptVertices += aiMesh->mNumVertices;
            _loadMesh(p_path, scene, mesh, meshName);

            std::cout << "-- [DONE] " << triMesh->getNbTriangles() << " triangles, " << triMesh->getNbVertices() << " vertices." << std::endl;
        }
        std::cout << "[DONE] " << scene->mNumMeshes << " meshes, " << cptTriangles << " triangles, " << cptVertices
                  << " vertices." << std::endl;
    }

} // namespace Model
} // namespace TutoVulkan
