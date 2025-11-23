#pragma once

#include "sub_mesh.hpp" 
#include "scene/objects/object.hpp"

#include <vector>

namespace M3D
{
	class Mesh : public Object
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Mesh(std::vector<SubMesh>& p_subMeshes) : Object(), _subMeshes(p_subMeshes) {}
		~Mesh() {}

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline std::vector<SubMesh>& getSubMeshes() { return _subMeshes; }

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		std::vector<SubMesh> _subMeshes;
	};
}
