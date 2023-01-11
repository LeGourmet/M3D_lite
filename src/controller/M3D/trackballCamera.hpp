#ifndef __TRACKBALL_HPP__
#define __TRACKBALL_HPP__

#include "baseCamera.hpp"

namespace M3D_ISICG
{
	class TrackballCamera : public BaseCamera{
	  public:
		TrackballCamera() : BaseCamera() {}

		void moveFront( const float p_delta ) override;
		void moveRight( const float p_delta ) override;
		void moveUp( const float p_delta ) override;
		void rotate( const float p_yaw, const float p_pitch ) override;
		void setLookAt( const Vec3f & p_lookAt ) override;

	  private:
		Vec3f _center = Vec3f( 0.f, 0.f, 0.f );
	};
} // namespace M3D_ISICG

#endif // __TRACKBALL_HPP__
