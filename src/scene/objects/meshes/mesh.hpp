#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "sub_mesh.hpp" 
#include "scene/objects/object.hpp"
#include "scene/acceleration_structures/AABB.hpp"

#include <vector>

namespace M3D
{
    namespace Scene
    {
        class Mesh : public Object
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Mesh(std::vector<SubMesh>& p_subMeshes) { this->_subMeshes = p_subMeshes; }
            ~Mesh() { }

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            inline std::vector<SubMesh>& getSubMeshes() { return _subMeshes; }
            inline std::vector<AABB>& getInstancesAABB() { return _instancesAABB; }

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            void addInstance(SceneGraphNode* p_node) override { 
                Object::addInstance(p_node);
                _instancesAABB.push_back(AABB());
                updateIntanceAABB((unsigned int)_instances.size()-1);
            }

            void updateIntanceAABB(unsigned int p_instanceId) {
                if (p_instanceId >= _instances.size()) return;
                
                _instancesAABB[p_instanceId].reset();
                for (int i=0; i<_subMeshes.size() ;i++) 
                    for (int j=0; j<_subMeshes[i].getVertices().size() ;j++) {
                        Vec4f pos = (_instances[p_instanceId]->getTransformation() * Vec4f(_subMeshes[i].getVertices()[j]._position, 1.));
                        _instancesAABB[p_instanceId].extend(Vec3f(pos.x, pos.y, pos.z));
                    }
            }

        private:
            // ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            std::vector<SubMesh> _subMeshes;
            std::vector<AABB>    _instancesAABB;
        };
    }
}

#endif
