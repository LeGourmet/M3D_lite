#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "utils/define.hpp"
#include "scene/objects/object.hpp"

#include <string>
#include <cmath>

namespace M3D
{
	enum LIGHT_TYPE { POINT, SPOT, DIRECTINAL };

namespace Scene
{
	class Light : public Object
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Light(LIGHT_TYPE p_type, const Vec3f& p_color, const float p_intensity, const float p_innerConeAngle, const float p_outerConeAngle ) 
			: _type(p_type), _color(p_color), _intensity(p_intensity), _innerConeAngle(p_innerConeAngle), _outerConeAngle(p_outerConeAngle) {
			_range = (float)std::sqrt(256.*std::max(_color.x*_intensity,std::max(_color.y*_intensity,_color.z*_intensity)));
		}

		Light(LIGHT_TYPE p_type, const Vec3f& p_color, const float p_intensity) 
			: _type(p_type), _color(p_color), _intensity(p_intensity) {
			_range = (float)std::sqrt(256. * std::max(_color.x * _intensity, std::max(_color.y * _intensity, _color.z * _intensity)));
		}

		~Light(){}

		// ----------------------------------------------------- GETTERS -------------------------------------------------------
		inline LIGHT_TYPE getType() const { return _type; }

		inline const Vec3f& getColor() const { return _color; }
		inline float getIntensity() const { return _intensity; }
		inline const Vec3f getEmissivity() const { return _color*_intensity; }
		inline float getRange() const { return _range; }

		inline float getInnerConeAngle() const { return _innerConeAngle; }
		inline float getOuterConeAngle() const { return _outerConeAngle; }

	protected:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		LIGHT_TYPE _type;

		Vec3f _color = VEC3F_ONE;
		float _intensity = 0.;
		float _range = 0.;

		float _innerConeAngle = 1.;
		float _outerConeAngle = -1.;
	};
}
}

#endif