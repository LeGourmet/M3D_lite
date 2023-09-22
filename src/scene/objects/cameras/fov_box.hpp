#ifndef __FOV_BOX_HPP__
#define __FOV_BOX_HPP__

#include "utils/define.hpp"

namespace M3D
{
	namespace Scene
	{
		class FovBox {
		public :
			FovBox() {}
			~FovBox() {}

            // 0=>left ; 1=>right ; 2=>down ; 3=>up ; 4=>near ; 5=>far
			void update(Mat4f& p_invVP) {
                Vec4f tmp;
                tmp = p_invVP * Vec4f(1.f, 1.f, 1.f, 1.f);    _points[0] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;
                tmp = p_invVP * Vec4f(-1.f, 1.f, 1.f, 1.f);   _points[1] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;
                tmp = p_invVP * Vec4f(1.f, -1.f, 1.f, 1.f);   _points[2] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;
                tmp = p_invVP * Vec4f(-1.f, -1.f, 1.f, 1.f);  _points[3] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;
                tmp = p_invVP * Vec4f(1.f, 1.f, -1.f, 1.f);   _points[4] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;
                tmp = p_invVP * Vec4f(-1.f, 1.f, -1.f, 1.f);  _points[5] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;
                tmp = p_invVP * Vec4f(1.f, -1.f, -1.f, 1.f);  _points[6] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;
                tmp = p_invVP * Vec4f(-1.f, -1.f, -1.f, 1.f); _points[7] = Vec3f(tmp.x, tmp.y, tmp.z) / tmp.w;

                _normals[0] = glm::normalize(glm::cross(_points[3] - _points[7], _points[5] - _points[7]));
                _normals[1] = glm::normalize(glm::cross(_points[6] - _points[2], _points[0] - _points[2]));
                _normals[2] = glm::normalize(glm::cross(_points[2] - _points[6], _points[7] - _points[6]));
                _normals[3] = glm::normalize(glm::cross(_points[0] - _points[1], _points[5] - _points[1]));
                _normals[4] = glm::normalize(glm::cross(_points[7] - _points[6], _points[4] - _points[6]));
                _normals[5] = glm::normalize(glm::cross(_points[2] - _points[3], _points[1] - _points[3]));

                _midPoints[0] = (_points[3] + _points[5]) * 0.5f;
                _midPoints[1] = (_points[2] + _points[4]) * 0.5f;
                _midPoints[2] = (_points[3] + _points[6]) * 0.5f;
                _midPoints[3] = (_points[1] + _points[4]) * 0.5f;
                _midPoints[4] = (_points[5] + _points[6]) * 0.5f;
                _midPoints[5] = (_points[1] + _points[2]) * 0.5f;

                float h = glm::distance(_midPoints[4], _midPoints[5]);
                float s1 = glm::distance(_points[4], _points[5]) * glm::distance(_points[4], _points[6]);
                float s2 = glm::distance(_points[0], _points[1]) * glm::distance(_points[0], _points[2]);
                _area = h * (s1 + s2 + sqrt(s1 * s2)) / 3.f;
			}

			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
            Vec3f _points[8] = {VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO};
			Vec3f _normals[6] = {VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO};
			Vec3f _midPoints[6] = {VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO,VEC3F_ZERO};
			float _area = 0;
		};
	}
}

#endif
