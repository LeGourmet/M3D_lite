#include "baseCamera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include <iostream>

namespace M3D_ISICG
{
	void BaseCamera::setScreenSize( const int p_width, const int p_height )
	{
		_screenWidth  = p_width;
		_screenHeight = p_height;
		_aspectRatio  = float( _screenWidth ) / _screenHeight;
		_updateVectors();
		_computeViewMatrix();
		_computeProjectionMatrix();
	}

	void BaseCamera::print() const
	{
		std::cout << "======== Camera ========" << std::endl;
		std::cout << "Position: " << glm::to_string( _position ) << std::endl;
		std::cout << "View direction: " << glm::to_string( -_invDirection ) << std::endl;
		std::cout << "Right: " << glm::to_string( _right ) << std::endl;
		std::cout << "Up: " << glm::to_string( _up ) << std::endl;
		std::cout << "Yaw: " << _yaw << std::endl;
		std::cout << "Pitch: " << _pitch << std::endl;
		std::cout << "========================" << std::endl;
	}

	void BaseCamera::setPosition( const Vec3f & p_position )
	{
		_position = p_position;
		_computeViewMatrix();
	}

	void BaseCamera::setFovy( const float p_fovy )
	{
		_fovy = p_fovy;
		_computeProjectionMatrix();
	}

	void BaseCamera::setCameraMode( const cameraMode mode )
	{
		_cameraMode = mode;
		_computeProjectionMatrix();
	}

	void BaseCamera::_computeViewMatrix() { _viewMatrix = glm::lookAt( _position, _position - _invDirection, _up ); }

	void BaseCamera::_computeProjectionMatrix()
	{
		_projectionMatrix = ( _cameraMode == cameraMode::ortho
								  ? glm::ortho( _aspectRatio,-1.f*_aspectRatio,-1.f*_aspectRatio,_aspectRatio,_zNear,_zFar )
								  : glm::perspective( glm::radians( _fovy ), _aspectRatio, _zNear, _zFar ));
	}

	void BaseCamera::_updateVectors()
	{
		const float yaw	  = glm::radians( _yaw );
		const float pitch = glm::radians( _pitch );
		_invDirection  = glm::normalize(
			Vec3f( glm::cos( yaw ) * glm::cos( pitch ), glm::sin( pitch ), glm::sin( yaw ) * glm::cos( pitch ) ) );
		_right = glm::normalize( glm::cross( Vec3f( 0.f, 1.f, 0.f ), _invDirection ) ); // We suppose 'y' as up.
		_up	   = glm::normalize( glm::cross( _invDirection, _right ) );

		_computeViewMatrix();
	}
} // namespace M3D_ISICG