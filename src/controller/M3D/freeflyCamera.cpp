#include "freeflyCamera.hpp"

namespace M3D_ISICG
{
	void FreeflyCamera::moveFront( const float p_delta ){
		_position -= _invDirection * p_delta;
		_computeViewMatrix();
	}

	void FreeflyCamera::moveRight( const float p_delta ){
		_position += _right * p_delta;
		_computeViewMatrix();
	}

	void FreeflyCamera::moveUp( const float p_delta ){
		_position += _up * p_delta;
		_computeViewMatrix();
	}

	void FreeflyCamera::rotate( const float p_yaw, const float p_pitch ){
		_yaw   = glm::mod( _yaw + p_yaw, 360.f );
		_pitch = glm::clamp( _pitch + p_pitch, -89.f, 89.f );
		_updateVectors();
	}

	void FreeflyCamera::setLookAt( const Vec3f & p_lookAt )
	{
		Vec3f newInvDir = glm::normalize( _position - p_lookAt );
		_pitch			= glm::degrees( glm::acos( glm::dot( newInvDir, _invDirection ) ) );
		_yaw			= glm::degrees( glm::acos( glm::dot( newInvDir, _right ) ) );
		_updateVectors();
	}
} // namespace M3D_ISICG