#ifndef __VERTEX_HPP__
#define __VERTEX_HPP__

#include "utils/define.hpp"

namespace M3D {
    typedef struct Vertex {
        Vec3f _position;
        Vec3f _normal;
        Vec2f _uvs;
        Vec3f _tangent;
        Vec3f _bitangent;
    } Vertex;
}
#endif