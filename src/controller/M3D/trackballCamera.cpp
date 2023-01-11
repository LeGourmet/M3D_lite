#include "trackballCamera.hpp"

namespace M3D_ISICG
{
	void TrackballCamera::moveFront( const float p_delta ){ 
		float r = glm::sqrt( ((_position.x-_center.x) * (_position.x-_center.x))
						   + ((_position.y-_center.y) * (_position.y-_center.y))
						   + ((_position.z-_center.z) * (_position.z-_center.z)) );
		_position =   r * glm::cos(glm::degrees(-p_delta)) * _invDirection
					+ r * glm::sin(glm::degrees(-p_delta)) * _up;
		
		_invDirection = glm::normalize( _position - _center );
		_right		  = glm::normalize( glm::cross( Vec3f( 0.f, (_position.z <= 0.f ? -1.f : 1.f), 0.f ), _invDirection ) );
		_up			  = glm::normalize( glm::cross( _invDirection, _right ) );
		_computeViewMatrix();
		//setLookAt( _center );
	}

	void TrackballCamera::moveRight( const float p_delta ){
		float r = glm::sqrt( ((_position.x-_center.x) * (_position.x-_center.x))
						   + ((_position.y-_center.y) * (_position.y-_center.y)) 
						   + ((_position.z-_center.z) * (_position.z-_center.z)) );
		
		_position =   r * glm::cos(glm::degrees(-p_delta)) * _invDirection
					+ r * glm::sin(glm::degrees(-p_delta)) * _right;
		setLookAt(_center);
	}

	void TrackballCamera::moveUp( const float p_delta )
	{
		_position -= _invDirection * p_delta;
		_computeViewMatrix();
	}

	void TrackballCamera::rotate( const float p_yaw, const float p_pitch ){}

	void TrackballCamera::setLookAt( const Vec3f & p_lookAt ){
		_center	= p_lookAt;
		_invDirection = glm::normalize( _position - p_lookAt );
		_right = glm::normalize( glm::cross( Vec3f( 0.f, 1.f, 0.f ), _invDirection ) );
		_up = glm::normalize( glm::cross( _invDirection, _right ) );
		_computeViewMatrix();
	}
} // namespace M3D_ISICG