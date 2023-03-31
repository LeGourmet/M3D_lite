#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "utils/define.hpp"
#include "utils/image.hpp"

namespace M3D
{
namespace Scene
{
    class Material
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        Material(const Vec4f& p_baseColor, const Vec3f& p_emissivity, float p_metalness, float p_roughness,
                 Image* p_baseColorMap, Image* p_metalnessRoughnessMap, Image* p_normalMap, Image* p_occlusionMap, Image* p_emissivityMap):
            _baseColor(p_baseColor), _emissivity(p_emissivity), _metalness(p_metalness), _roughness(p_roughness),
            _baseColorMap(p_baseColorMap), _metalnessRoughnessMap(p_metalnessRoughnessMap), _normalMap(p_normalMap), _occlusionMap(p_occlusionMap), _emissivityMap(p_emissivityMap) 
        {
            _isOpaque = (_baseColorMap != nullptr && _baseColorMap->getNbChannels() < 4) || (_baseColorMap == nullptr && _baseColor.a == 1.);
            _isEmissive = (_emissivityMap != nullptr) || (_emissivityMap == nullptr && glm::length(_emissivity) > 0.f);
        }
        ~Material(){}

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        inline const Vec4f& getBaseColor() const { return _baseColor; }
        inline const Vec3f& getEmissivity() const { return _emissivity; }
        inline float getMetalness() const { return _metalness; }
        inline float getRoughness() const { return _roughness; }
        inline bool isOpaque() const { return _isOpaque; }
        inline bool isEmissive() const { return _isEmissive; }

        inline Image* getBaseColorMap() const { return _baseColorMap; }
        inline Image* getMetalnessRoughnessMap() const { return _metalnessRoughnessMap; }
        inline Image* getNormalMap() const { return _normalMap; }
        inline Image* getOcclusionMap() const { return _occlusionMap; }
        inline Image* getEmissivityMap() const { return _emissivityMap; }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        // add emissivity power (strenth) ? + ior
        
        Vec4f _baseColor = VEC4F_ONE;
        Vec3f _emissivity = VEC3F_ZERO;
        float _metalness = 0.;
        float _roughness = 1.;
        bool _isOpaque = true;
        bool _isEmissive = false; 

        Image* _baseColorMap = nullptr;
        Image* _metalnessRoughnessMap = nullptr;
        Image* _normalMap = nullptr;
        Image* _occlusionMap = nullptr;
        Image* _emissivityMap = nullptr;
    };
}
}

#endif