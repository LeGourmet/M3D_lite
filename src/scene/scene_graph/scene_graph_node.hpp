#ifndef __SCENE_GRAPH_NODE_HPP__
#define __SCENE_GRAPH_NODE_HPP__

#include "utils/define.hpp"

namespace M3D
{
namespace Scene
{
    class SceneGraphNode
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        SceneGraphNode(){}
        ~SceneGraphNode(){}

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        Mat4f computeTransformation() const {
            Mat4f result = MAT4F_ID;
            SceneGraphNode* current = _parent;

            while (current != nullptr) result = current->_transformation * result;

            return result;
        }

    public:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        SceneGraphNode* _parent = nullptr;
        Mat4f _transformation = MAT4F_ID;
    };
}
}

#endif