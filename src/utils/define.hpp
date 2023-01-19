#ifndef __DEFINE_HPP__
#define __DEFINE_HPP__

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <string>
#include <filesystem>
#include <limits>

//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace M3D
{
	// Types.
	using Vec2f = glm::vec2;
	using Vec2i = glm::ivec2;
	using Vec2u = glm::uvec2;
	using Vec3f = glm::vec3;
	using Vec3i = glm::ivec3;
	using Vec3u = glm::uvec3;
	using Vec3d = glm::dvec3;
	using Vec4f = glm::vec4;

	using Mat3f = glm::mat3;
	using Mat43f = glm::mat4x3;
	using Mat3d = glm::dmat3;
	using Mat4f = glm::mat4;
	using Mat4d = glm::dmat4;

	using Quatf = glm::quat;
	using Quatd = glm::dquat;

	using Path = std::filesystem::path;

	// Constants.
	constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
	constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
	constexpr float FLOAT_INF = std::numeric_limits<float>::infinity();

	constexpr float PIf = glm::pi<float>();
	constexpr float PI_2f = glm::half_pi<float>();
	constexpr float PI_4f = glm::quarter_pi<float>();
	constexpr float PI_32f = glm::three_over_two_pi<float>();
	constexpr float TWO_PIf = glm::two_pi<float>();
	constexpr float INV_PIf = glm::one_over_pi<float>();
	constexpr float INV_2PIf = glm::one_over_two_pi<float>();

	constexpr Vec3f VEC3F_ZERO = Vec3f(0.f, 0.f, 0.f);
	constexpr Vec3f VEC3F_X = Vec3f(1.f, 0.f, 0.f);
	constexpr Vec3f VEC3F_Y = Vec3f(0.f, 1.f, 0.f);
	constexpr Vec3f VEC3F_Z = Vec3f(0.f, 0.f, 1.f);
	constexpr Vec3f VEC3F_XYZ = Vec3f(1.f, 1.f, 1.f);
	constexpr Vec3f VEC3F_MIN = Vec3f(FLOAT_MIN);
	constexpr Vec3f VEC3F_MAX = Vec3f(FLOAT_MAX);

	constexpr Mat3f MAT3F_ID = Mat3f(1.f);
	constexpr Mat4f MAT4F_ID = Mat4f(1.f);

	typedef struct Vertex {
		Vec3f _position;
		Vec3f _normal;
		Vec2f _uvs;
		Vec3f _tangent;
		Vec3f _bitangent;
	} Vertex;
}

#endif