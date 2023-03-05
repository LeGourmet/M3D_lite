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
        Material(Vec4f p_baseColor, Vec3f p_emissivity, float p_metalness, float p_roughness, bool p_isOpaque, 
                 Image* p_baseColorMap, Image* p_metalnessRougthnessMap, Image* p_normalMap, Image* p_occlusionMap, Image* p_emissivityMap):
            _baseColor(p_baseColor), _emissivity(p_emissivity), _metalness(p_metalness), _roughness(p_roughness), _isOpaque(p_isOpaque),
            _baseColorMap(p_baseColorMap), _metalnessRougthnessMap(p_metalnessRougthnessMap), _normalMap(p_normalMap), _occlusionMap(p_occlusionMap), _emissivityMap(p_emissivityMap) {}
        ~Material(){}

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        inline const Vec4f& getBaseColor() const { return _baseColor; }
        inline const Vec3f& getEmissivity() const { return _emissivity; }
        inline const float getMetalness() const { return _metalness; }
        inline const float getRoughness() const { return _roughness; }
        inline const bool isOpaque() const { return _isOpaque; }

        inline const Image* getBaseColorMap() const { return _baseColorMap; }
        inline const Image* getMetalnessRougthnessMap() const { return _metalnessRougthnessMap; }
        inline const Image* getNormalMap() const { return _normalMap; }
        inline const Image* getOcclusionMap() const { return _occlusionMap; }
        inline const Image* getEmissivityMap() const { return _emissivityMap; }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        Vec4f _baseColor = VEC4F_ONE;
        Vec3f _emissivity = VEC3F_ZERO;
        float _metalness = 0.;
        float _roughness = 1.;
        bool _isOpaque = true;

        // ior
        // emissivity power (strenth)

        Image* _baseColorMap = nullptr;
        Image* _metalnessRougthnessMap = nullptr;
        Image* _normalMap = nullptr;
        Image* _occlusionMap = nullptr;
        Image* _emissivityMap = nullptr;
    };
}
}

#endif