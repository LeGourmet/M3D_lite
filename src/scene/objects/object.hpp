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

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            inline const unsigned int getNumberInstances() const { return (unsigned int)_instances.size(); }
            SceneGraphNode* getInstance(unsigned int p_instanceId) {
                if (p_instanceId < _instances.size()) return _instances[p_instanceId];
                return nullptr;
            }

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void addInstance(SceneGraphNode* p_node) { _instances.push_back(p_node); }

        protected:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            std::vector<SceneGraphNode*> _instances;
        };
    }
}

#endif
