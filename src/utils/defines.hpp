#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/detail/type_quat.hpp"

#include <limits>

namespace M3D
{
	// Types.
	using uchar = uint8_t;
	using ushort = uint16_t;
	using uint = uint32_t;

	using ulong = uint64_t;
	using slong = int64_t;

	using Vec2b = glm::bvec2;
	using Vec2f = glm::vec2;
	using Vec2d = glm::dvec2;
	using Vec2i = glm::ivec2;
	using Vec2u = glm::uvec2;
	using Vec3b = glm::bvec3;
	using Vec3f = glm::vec3;
	using Vec3d = glm::dvec3;
	using Vec3i = glm::ivec3;
	using Vec3u = glm::uvec3;
	using Vec4b = glm::bvec4;
	using Vec4f = glm::vec4;
	using Vec4d = glm::dvec4;
	using Vec4i = glm::ivec4;
	using Vec4u = glm::uvec4;

	using Mat2f = glm::mat2;
	using Mat3f = glm::mat3;
	using Mat4f = glm::mat4;

	using Quatf = glm::quat;

	// Constants.
	constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
	constexpr float FLOAT_MAX = std::numeric_limits<float>::max();

	constexpr float PIf = glm::pi<float>();
	constexpr float PI_2f = glm::half_pi<float>();
	constexpr float PI_4f = glm::quarter_pi<float>();
	constexpr float TWO_PIf = glm::two_pi<float>();
	constexpr float INV_PIf = glm::one_over_pi<float>();
	constexpr float INV_2PIf = glm::one_over_two_pi<float>();

	constexpr Vec3f VEC3F_ONE = Vec3f(1.f, 1.f, 1.f);
	constexpr Vec3f VEC3F_ZERO = Vec3f(0.f, 0.f, 0.f);
	constexpr Vec3f VEC3F_X = Vec3f(1.f, 0.f, 0.f);
	constexpr Vec3f VEC3F_Y = Vec3f(0.f, 1.f, 0.f);
	constexpr Vec3f VEC3F_Z = Vec3f(0.f, 0.f, 1.f);
	constexpr Vec3f VEC3F_MIN = Vec3f(FLOAT_MIN);
	constexpr Vec3f VEC3F_MAX = Vec3f(FLOAT_MAX);

	constexpr Vec4f VEC4F_ONE = Vec4f(1.f, 1.f, 1.f, 1.f);
	constexpr Vec4f VEC4F_ZERO = Vec4f(0.f, 0.f, 0.f, 0.f);

	constexpr Mat3f MAT3F_ID = Mat3f(1.f);
	constexpr Mat4f MAT4F_ID = Mat4f(1.f);

	constexpr Quatf QUATF_ID = Quatf(1.0, { 0.0, 0.0, 0.0 });
}
