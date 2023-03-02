#ifndef __POINT_LIGHT_HPP__
#define __POINT_LIGHT_HPP__

#include "light.hpp"

namespace M3D
{
	class PointLight : public Light
	{
	public:
		PointLight(const Vec3f& p_position, const Vec3f& p_color, const float p_intensity, const float p_range)
			: Light(p_position, p_color, p_intensity, p_range, LIGHT_TYPE::POINT) {}
		~PointLight() {}
	};

}

#endif