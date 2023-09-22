#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "utils/define.hpp"
#include "scene/objects/cameras/fov_box.hpp"

namespace M3D
{
	namespace Scene
	{
		class AABB
		{
		public:
			// --------------------------------------------- CONSTRUCTOR / DESTRUCTOR ----------------------------------------------
			AABB() { this->reset(); }
			~AABB() {}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			inline const Vec3f& getMin() const { return _min; }
			inline const Vec3f& getMax() const { return _max; }
			inline Vec3f getDiagonal() const { return _max - _min; }
			inline Vec3f getCentroid() const { return (_min + _max) * 0.5f; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------     
			void reset() {
				_min = Vec3f(FLT_MAX);
				_max = Vec3f(-FLT_MAX);
			}
			
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

			bool intersectSphere(Vec3f p_sphereCenter, float p_sphereRadius){
				float dmin = 0.f;
				for (int i=0; i<3 ;i++) {
						 if (p_sphereCenter[i] < _min[i]) { dmin += (p_sphereCenter[i]-_min[i])*(p_sphereCenter[i]-_min[i]); }
					else if (p_sphereCenter[i] > _max[i]) { dmin += (p_sphereCenter[i]-_max[i])*(p_sphereCenter[i]-_max[i]); }
				}
				return dmin <= p_sphereRadius*p_sphereRadius;
			}

			// intersect cone +/ sphere cut with cone 

			// peut avoir un peut de faux positif mais il ne me semble pas qu'il y ai de faux négatif
			bool intersectFrustum(FovBox& p_fovbox, Mat4f& p_transformations) {
				for (int i=0; i<6 ;i++) // check it
					if( _isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_min.x,_min.y,_min.z)) &&
						_isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_max.x,_min.y,_min.z)) &&
						_isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_min.x,_max.y,_min.z)) &&
						_isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_max.x,_max.y,_min.z)) &&
						_isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_min.x,_min.y,_max.z)) &&
						_isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_max.x,_min.y,_max.z)) &&
						_isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_min.x,_max.y,_max.z)) &&
						_isInsidePlane(p_fovbox._normals[i], p_fovbox._midPoints[i], Vec3f(_max.x,_max.y,_max.z)) )
							return false;

				for(int i=0; i<3 ;i++){
					if (p_fovbox._points[0][i]>_max[i] && p_fovbox._points[1][i]>_max[i] &&
						p_fovbox._points[2][i]>_max[i] && p_fovbox._points[3][i]>_max[i] &&
						p_fovbox._points[4][i]>_max[i] && p_fovbox._points[5][i]>_max[i] &&
						p_fovbox._points[6][i]>_max[i] && p_fovbox._points[7][i]>_max[i]) return false;
					if (p_fovbox._points[0][i]<_min[i] && p_fovbox._points[1][i]<_min[i] &&
						p_fovbox._points[2][i]<_min[i] && p_fovbox._points[3][i]<_min[i] &&
						p_fovbox._points[4][i]<_min[i] && p_fovbox._points[5][i]<_min[i] &&
						p_fovbox._points[6][i]<_min[i] && p_fovbox._points[7][i]<_min[i]) return false;
				}

				return true;
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			Vec3f _min;
			Vec3f _max;

			bool _isInsidePlane(Vec3f p_planeN, Vec3f p_planeO, Vec3f p_point) {
				return glm::dot(p_planeN, p_point - p_planeO) < 0.;
			}
		};
	}
}

#endif