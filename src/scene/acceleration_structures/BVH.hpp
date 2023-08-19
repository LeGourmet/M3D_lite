#ifndef __BVH_HPP__
#define __BVH_HPP__

#include "aabb.hpp"
#include "scene/objects/meshes/sub_mesh.hpp"
#include <vector>

namespace M3D
{
	namespace Scene 
	{
		struct Bucket {
			int _nb = 0;
			AABB _aabb;
		};

		struct BVHNode
		{
			BVHNode() { }
			~BVHNode() { delete _left; delete _right; }

			bool isLeaf() const { return (_left == nullptr && _right == nullptr); }

			AABB		 _aabb			  = AABB();
			BVHNode*	 _left			  = nullptr;
			BVHNode*	 _right			  = nullptr;
			unsigned int _firstTriangleId = 0;
			unsigned int _lastTriangleId  = 0;
		};

		class BVH
		{
			public:
				BVH() { }
				~BVH() { delete _root; }

				//void build(std::vector<TriangleMeshGeometry>* p_triangles) override;

			private:
				/*void _buildRec(BVHNode* p_node,
					const unsigned int p_firstTriangleId,
					const unsigned int p_nbTriangles,
					const unsigned int p_depth);*/

			private:
				BVHNode* _root = nullptr;
				std::vector<SubMesh*> _SubMeshes;

				const unsigned int _maxSubMeshesPerLeaf = 8;
				const unsigned int _maxDepth = 32;
		};
	}
}

#endif