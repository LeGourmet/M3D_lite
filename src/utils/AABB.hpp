#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "define.hpp"

namespace M3D
{
	class AABB
	{

	public:

		AABB() = default;
		AABB(const Vec3f& p_min, const Vec3f& p_max) : _min(p_min), _max(p_max) {}
		~AABB() = default;

		inline const Vec3f& getMin() const { return _min; }
		inline const Vec3f& getMax() const { return _max; }

		// Returns true if the AABB is degenerated.
		inline bool isValid() const { return ((_min.x <= _max.x) && (_min.y <= _max.y) && (_min.z <= _max.z)); }

		// Extends the AABB with a point
		inline void extend(const Vec3f& p_v)
		{
			_min = glm::min(_min, p_v);
			_max = glm::max(_max, p_v);
		}

		// Extends the AABB with another AABB
		inline void extend(const AABB& p_aabb)
		{
			_min = glm::min(_min, p_aabb.getMin());
			_max = glm::max(_max, p_aabb.getMax());
		}

		// Returns the AABB diagonal vector.
		inline Vec3f diagonal() const { return _max - _min; }

		// Returns the AABB surface. 
		inline float surface()
		{
			const Vec3f d = diagonal();
			return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		// Returns the AABB centroid.
		inline Vec3f centroid() const { return (_min + _max) * 0.5f; }

		// Returns the largest axis, 0 -> x, 1 -> y, 2 -> z
		inline size_t largestAxis() const
		{
			const Vec3f d = diagonal();
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}

		/*inline bool isOnFrustrum() {
			//Get global scale thanks to our transform
			const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

			// Scaled orientation
			const glm::vec3 right = transform.getRight() * extents.x;
			const glm::vec3 up = transform.getUp() * extents.y;
			const glm::vec3 forward = transform.getForward() * extents.z;

			const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

			const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

			const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

			//We not need to divise scale because it's based on the half extention of the AABB
			const AABB globalAABB(globalCenter, newIi, newIj, newIk);

			return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
					globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
					globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
					globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
					globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
					globalAABB.isOnOrForwardPlane(camFrustum.farFace));
		}

		bool isOnOrForwardPlane(const Plane& plane) const
		{
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			const float r = extents.x * std::abs(plane.normal.x) +
				extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

			return -r <= plane.getSignedDistanceToPlane(center);
		}*/

	private:
		Vec3f _min = Vec3f(FLT_MAX);
		Vec3f _max = Vec3f(-FLT_MAX);
	};
}
#endif