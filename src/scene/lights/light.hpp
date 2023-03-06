#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "utils/define.hpp"

#include <string>
#include <cmath>

namespace M3D
{
	enum LIGHT_TYPE { POINT, SPOT, DIRECTINAL };

namespace Scene
{
	class Light
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Light(const std::string p_type, const Vec3f& p_color, const float p_intensity, const float p_innerConeAngle, const float p_outerConeAngle ) : _color(p_color), _intensity(p_intensity) {
			if (p_type == "spot") {
				_type = LIGHT_TYPE::SPOT;
				_innerConeAngle = p_innerConeAngle;
				_outerConeAngle = p_outerConeAngle;
			} else { _type = ((p_type == "point") ? LIGHT_TYPE::POINT : LIGHT_TYPE::DIRECTINAL); }
			
			_range = (float)std::sqrt(256.*std::max(_color.x*_intensity,std::max(_color.y*_intensity,_color.z*_intensity)));
		}
		~Light(){}

		// ----------------------------------------------------- GETTERS -------------------------------------------------------
		const LIGHT_TYPE getType() const { return _type; }

		const Vec3f& getPosition() const { return _position; }
		const Vec3f& getDirection() const { return _direction; }

		const Vec3f& getColor() const { return _color; }
		const float getIntensity() const { return _intensity; }
		const Vec3f getEmissivity() const { return _color*_intensity; }
		const float getRange() const { return _range; }

		const float getInnerConeAngle() const { return _innerConeAngle; }
		const float getOuterConeAngle() const { return _outerConeAngle; }

	protected:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		LIGHT_TYPE _type;

		Vec3f _position = VEC3F_ZERO;    // to delete and set ModelMatrix
		Vec3f _direction = VEC3F_Z;		 // to delete and set ModelMatrix

		Vec3f _color = VEC3F_ONE;
		float _intensity = 0.;
		float _range = 0.;

		float _innerConeAngle = 1.;
		float _outerConeAngle = -1.;
	};
}
}

#endif