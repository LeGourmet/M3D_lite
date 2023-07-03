#ifndef __VERTEX_HPP__
#define __VERTEX_HPP__

#include "utils/define.hpp"

namespace M3D
{
	namespace Scene
	{
		struct Vertex {
			Vec3f _position;
			Vec3f _normal;
			Vec2f _uv;
			Vec3f _tangent;
			Vec3f _bitangent;
		};
	}
}

#endif
