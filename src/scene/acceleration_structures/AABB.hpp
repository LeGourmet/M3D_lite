#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "utils/define.hpp"

namespace M3D
{
	namespace Scene
	{
		class AABB
		{
		public:
			// --------------------------------------------- CONSTRUCTOR / DESTRUCTOR ----------------------------------------------
			AABB() {}
			~AABB() {}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			inline const Vec3f& getMin() const { return _min; }
			inline const Vec3f& getMax() const { return _max; }
			inline Vec3f getDiagonal() const { return _max - _min; }
			inline Vec3f getCentroid() const { return (_min + _max) * 0.5f; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------     
			void extend(const Vec3f& p_v) {
				_min = glm::min(_min, p_v);
				_max = glm::max(_max, p_v);
			}

			void extend(const AABB& p_aabb) {
				_min = glm::min(_min, p_aabb.getMin());
				_max = glm::max(_max, p_aabb.getMax());
			}

			float computeArea() {
				const Vec3f d = getDiagonal();
				return d.x*d.y*d.z;
			}

			float computeSurface() {
				const Vec3f d = getDiagonal();
				return 2.f*(d.x*d.y + d.x*d.z + d.y*d.z);
			}

			int largestAxis() {
				const Vec3f d = getDiagonal();
				if (d.x >= d.y && d.x >= d.z) return 0;
				if (d.y >= d.y && d.y >= d.z) return 1;
											  return 2;
			}

			// inside frutum ?
			// collide ?
			// intersect ray ?

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			Vec3f _min = Vec3f(FLT_MAX);
			Vec3f _max = Vec3f(-FLT_MAX);
		};
	}
}

#endif