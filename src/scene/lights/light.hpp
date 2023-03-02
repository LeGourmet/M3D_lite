#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "utils/define.hpp"

namespace M3D
{
	enum LIGHT_TYPE {POINT,SPOT,DIRECTIONAL};

	class Light
	{
	public:
		Light(const Vec3f& p_position, const Vec3f& p_color, const float p_intensity, const float p_range, const LIGHT_TYPE p_type)
			: _position(p_position), _color(p_color), _intensity(p_intensity), _range(p_range), _type(p_type) {}
		virtual ~Light() = default;

		inline const Vec3f& getPosition() const { return _position; }
		inline const Vec3f& getColor() const { return _color; }
		inline const float getIntensity() const { return _intensity; }
		inline const Vec3f& getEmissivity() const { return _color*_intensity; }
		inline const float getRange() const { return _range; }
		inline const LIGHT_TYPE getType() const { return _type; }

	protected:
		Vec3f _position = VEC3F_ZERO;
		Vec3f _color = VEC3F_ONE;
		float _intensity = 0.;
		float _range = 0.;
		LIGHT_TYPE _type;

	};

}

#endif