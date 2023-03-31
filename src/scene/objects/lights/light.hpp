#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "utils/define.hpp"
#include "scene/objects/object.hpp"

#include <string>

namespace M3D
{
	enum LIGHT_TYPE { POINT, SPOT, DIRECTIONAL };

namespace Scene
{
	class Light : public Object
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Light(LIGHT_TYPE p_type, const Vec3f& p_color, const float p_intensity, const float p_innerConeAngle, const float p_outerConeAngle ) 
			: _type(p_type), _color(p_color), _intensity(p_intensity) {
			_cosInnerConeAngle = glm::cos(p_innerConeAngle);
			_cosOuterConeAngle = glm::cos(p_outerConeAngle);
			_range = (float)glm::sqrt(256.*_intensity*glm::max<float>(_color.x,glm::max<float>(_color.y,_color.z)));
		}

		Light(LIGHT_TYPE p_type, const Vec3f& p_color, const float p_intensity) 
			: _type(p_type), _color(p_color), _intensity(p_intensity) {
			_range = (float)glm::sqrt(256.*_intensity*glm::max<float>(_color.x,glm::max<float>(_color.y,_color.z)));
		}

		~Light(){}

		// ----------------------------------------------------- GETTERS -------------------------------------------------------
		inline LIGHT_TYPE getType() const { return _type; }

		inline const Vec3f& getColor() const { return _color; }
		inline float getIntensity() const { return _intensity; }
		inline Vec3f getEmissivity() const { return _color*_intensity; }
		inline float getRange() const { return _range; }

		inline float getCosInnerConeAngle() const { return _cosInnerConeAngle; }
		inline float getCosOuterConeAngle() const { return _cosOuterConeAngle; }

	protected:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		LIGHT_TYPE _type;

		Vec3f _color = VEC3F_ONE;
		float _intensity = 0.;
		float _range = 0.;

		float _cosInnerConeAngle = -1.;
		float _cosOuterConeAngle = -1.;
	};
}
}

#endif