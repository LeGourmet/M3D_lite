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
        void updateLocalTransformation() {
            // _transfromation = ??
        }
        
        Mat4f computeTransformation() const {
            Mat4f result = MAT4F_ID;
            SceneGraphNode* current = _parent;

            while (current != nullptr) result = current->_transformation * result;

            return result;
        }

    public:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        SceneGraphNode* _parent = nullptr;
        Vec4f _rotation = VEC4F_ZERO;
        Vec3f _scale = VEC3F_ONE;
        Vec3f _translation = VEC3F_ZERO;
        Mat4f _transformation = MAT4F_ID;
    };
}
}

#endif