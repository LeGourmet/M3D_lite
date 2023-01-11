#ifndef __BASECAMERA_HPP__
#define __BASECAMERA_HPP__

#include "GL/gl3w.h"
#include "utils/define.hpp"

#include <iostream>

namespace M3D_ISICG
{
	enum class cameraMode{ perspective, ortho };

	class BaseCamera
	{
	  public:
		BaseCamera() = default;
		~BaseCamera() = default;

		inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
		inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }
		inline const cameraMode & getCameraMode() const { return _cameraMode; }
		inline const Vec3f & getPosition() const { return _position; }

		void setPosition( const Vec3f & p_position );
		virtual void setLookAt( const Vec3f & p_lookAt ) = 0;
		void setFovy( const float p_fovy );
		void setCameraMode( const cameraMode mode );

		void setScreenSize( const int p_width, const int p_height );

		virtual void moveFront( const float p_delta ) = 0;
		virtual void moveRight( const float p_delta ) = 0;
		virtual void moveUp( const float p_delta )	  = 0;
		virtual void rotate( const float p_yaw, const float p_pitch ) = 0;

		void print() const;

	  protected:
		void _computeViewMatrix();
		void _computeProjectionMatrix();
		void _updateVectors();

	  protected:
		Vec3f _position		= VEC3F_ZERO;
		Vec3f _invDirection = Vec3f( 0.f, 0.f, 1.f );  // Dw dans le cours.
		Vec3f _right		= Vec3f( -1.f, 0.f, 0.f ); // Rw dans le cours.
		Vec3f _up			= Vec3f( 0.f, 1.f, 0.f );  // Uw dans le cours.
		// Angles defining the orientation in degrees
		float _yaw	 = 90.f;
		float _pitch = 0.f;

		int	  _screenWidth	= 1280;
		int	  _screenHeight = 720;
		float _aspectRatio	= float( _screenWidth ) / _screenHeight;
		float _fovy			= 60.f;
		float _zNear		= 0.1f;
		float _zFar			= 1000.f;

		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

		cameraMode _cameraMode = cameraMode::perspective;
	};
} // namespace M3D_ISICG

#endif // __BASECAMERA_HPP__
