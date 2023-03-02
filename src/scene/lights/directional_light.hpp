#ifndef __DIRECTIONAL_LIGHT_HPP__
#define __DIRECTIONAL_LIGHT_HPP__

#include "light.hpp"

namespace M3D
{
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(const Vec3f& p_position, const Vec3f& p_color, const float p_intensity, const Vec3f& p_direction)
			: Light(p_position, p_color, p_intensity, 1.f, LIGHT_TYPE::DIRECTIONAL), _direction(p_direction) {}
		DirectionalLight(const Vec3f& p_color, const float p_intensity, const Vec3f& p_direction)
			: Light(VEC3F_ZERO, p_color, p_intensity, 1.f, LIGHT_TYPE::DIRECTIONAL), _direction(p_direction) {}
		~DirectionalLight() {}

		inline const Vec3f& getDirection() const { return _direction; }

	private:
		Vec3f _direction;
	};

}

#endif