#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "utils/define.hpp"
#include "texture.hpp"

namespace M3D
{
    namespace Scene
    {
        class Material
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Material(const Vec4f& p_baseColor, const Vec3f& p_emissiveColor, float p_emissiveStrength, float p_metalness, float p_roughness, float p_alphaCutOff, bool p_doubleSide,
                     Texture* p_baseColorMap, Texture* p_metalnessRoughnessMap, Texture* p_normalMap, Texture* p_occlusionMap, Texture* p_emissiveMap):
                _baseColor(p_baseColor), _emissiveColor(p_emissiveColor), _emissiveStrength(p_emissiveStrength), _metalness(p_metalness), _roughness(p_roughness), _alphCutOff(p_alphaCutOff), _isDoubleSide(!p_doubleSide), // the invert of the spec gltf
                _baseColorMap(p_baseColorMap), _metalnessRoughnessMap(p_metalnessRoughnessMap), _normalMap(p_normalMap), _occlusionMap(p_occlusionMap), _emissiveMap(p_emissiveMap) 
            {
                _isTransparent = (_baseColorMap != nullptr && _baseColorMap->_image->getNbChannels() == 4) || (_baseColorMap == nullptr && _baseColor.a < 1.f);
                _isOpaque = (_baseColorMap != nullptr) || (_baseColorMap == nullptr && _baseColor.a == 1.f);
                _isEmissive = (_emissiveMap != nullptr) || (_emissiveMap == nullptr && glm::length(getEmissivity()) > 0.f); // add thresold
            }
            ~Material(){}

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            // change for name albedo
            inline const Vec4f& getBaseColor() const { return _baseColor; }
            inline const Vec3f& getEmissiveColor() const { return _emissiveColor; }
            inline const Vec3f getEmissivity() const { return _emissiveColor*_emissiveStrength; }

            inline float getEmissiveStrength() const { return _emissiveStrength; }
            inline float getMetalness() const { return _metalness; }
            inline float getRoughness() const { return _roughness; }
            inline float getAlphaCutOff() const { return _alphCutOff; }

            inline bool isOpaque() const { return _isOpaque; }
            inline bool isTransparent() const { return _isTransparent; }
            inline bool isEmissive() const { return _isEmissive; }
            inline bool isDoubleSide() const { return _isDoubleSide; }

            inline Texture* getBaseColorMap() const { return _baseColorMap; }
            inline Texture* getMetalnessRoughnessMap() const { return _metalnessRoughnessMap; }
            inline Texture* getNormalMap() const { return _normalMap; }
            inline Texture* getOcclusionMap() const { return _occlusionMap; }
            inline Texture* getEmissiveMap() const { return _emissiveMap; }

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            Vec4f _baseColor = VEC4F_ONE;
            Vec3f _emissiveColor = VEC3F_ZERO;

            float _emissiveStrength = 0.f;
            float _metalness = 0.f;
            float _roughness = 1.f;
            float _alphCutOff = 0.f;  // (opaque no discard = 0.) / (blend full discard = 1.) / (mask partial discard = others)

            bool _isOpaque = true;
            bool _isTransparent = false;
            bool _isEmissive = false; 
            bool _isDoubleSide = false;

            Texture* _baseColorMap = nullptr;
            Texture* _metalnessRoughnessMap = nullptr;
            Texture* _normalMap = nullptr;
            Texture* _occlusionMap = nullptr; // ???
            Texture* _emissiveMap = nullptr;
        };
    }
}

#endif
