#ifndef __SCENE_GRAPH_NODE_HPP__
#define __SCENE_GRAPH_NODE_HPP__

#include "utils/define.hpp"

namespace M3D
{
namespace Scene
{
    struct SceneGraphNode{
        SceneGraphNode* _parent;
        Mat4f _transformation;
    };
}
}

#endif