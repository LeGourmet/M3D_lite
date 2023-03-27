#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include "utils/define.hpp"
#include "scene/scene_graph/scene_graph_node.hpp"

#include <vector>

namespace M3D
{
namespace Scene
{
    class Object
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
        Object() {}
        ~Object() {}

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        inline const std::vector<SceneGraphNode*>& getSceneGraphNode() const { return _instances; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void addInstance(SceneGraphNode* p_node) { _instances.push_back(p_node); }

    private:
        // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
        std::vector<SceneGraphNode*> _instances;
    };
}
}

#endif