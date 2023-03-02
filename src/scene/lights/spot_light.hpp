#ifndef __SPOT_LIGHT_HPP__
#define __SPOT_LIGHT_HPP__

#include "light.hpp"

namespace M3D
{
	class SpotLight : public Light
	{
	public:
		SpotLight(const Vec3f& p_position, const Vec3f& p_color, const float p_intensity, const float p_range, const Vec3f& p_direction, const float p_innerConeAngle, const float p_outerConeAngle)
			: Light(p_position, p_color, p_intensity, p_range, LIGHT_TYPE::SPOT), _direction(p_direction), _innerConeAngle(p_innerConeAngle), _outerConeAngle(p_outerConeAngle) {}
		~SpotLight() {}

		inline const Vec3f& getDirection() const { return _direction; }
		inline const float getInnerConeAngle() const { return _innerConeAngle; }
		inline const float getOuterConeAngle() const { return _outerConeAngle; }

	private:
		Vec3f _direction;
		float _innerConeAngle;
		float _outerConeAngle;
	};

}

#endif