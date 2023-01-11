#ifndef __FREEFLY_HPP__
#define __FREEFLY_HPP__

#include "baseCamera.hpp"

namespace M3D_ISICG
{
	class FreeflyCamera : public BaseCamera{
	  public:
		FreeflyCamera() : BaseCamera() {}

		void moveFront( const float p_delta ) override;
		void moveRight( const float p_delta ) override;
		void moveUp( const float p_delta ) override;
		void rotate( const float p_yaw, const float p_pitch ) override;
		void setLookAt( const Vec3f & p_lookAt ) override;
	};
} // namespace M3D_ISICG

#endif // __FREEFLY_HPP__