#pragma once

#include "utils/defines.hpp"
#include "scene/scene_graph_node.hpp"

#include <vector>

namespace M3D
{
	class Object
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Object() {}
		~Object() {}

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline const uint getNumberInstances() const { return (uint)_instances.size(); }

		SceneGraphNode* getInstance(uint p_instanceId) {
			if (p_instanceId < _instances.size()) return _instances[p_instanceId];
			return nullptr;
		}

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------           
		virtual void addInstance(SceneGraphNode* p_node) { _instances.push_back(p_node); }

	protected:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		std::vector<SceneGraphNode*> _instances;
	};
}
