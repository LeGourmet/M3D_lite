#pragma once

#include "glm/detail/_fixes.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "utils/defines.hpp"
#include "scene/objects/object.hpp"

namespace M3D
{
	enum CAMERA_TYPE { PERSPECTIVE, ORTHOGRAPHIC };

	class Camera : public Object
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		/*
		* p_arg1 is interpreted as fovy or xmag, it depend of the CAMERA_TYPE
		* p_arg2 is interpreted as aspect ratio or ymag, it depend of the CAMERA_TYPE
		*/
		Camera(float p_arg1, float p_arg2, float p_znear, float p_zfar, CAMERA_TYPE p_type) : Object(), _type(p_type) {
			switch (p_type) {
			case CAMERA_TYPE::PERSPECTIVE:  _fovy = p_arg1; _aspectRatio = p_arg2; break;
			case CAMERA_TYPE::ORTHOGRAPHIC: _xmag = p_arg1; _ymag = p_arg2;        break;
			}

			_znear = p_znear;
			_zfar = p_zfar;

			_updateProjectionMatrix();
		}
		~Camera() {}

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline CAMERA_TYPE getType()  const { return _type; }
		inline float getZNear()       const { return _znear; }
		inline float getZFar()        const { return _zfar; }
		inline float getFovy()        const { return _fovy; }
		inline float getAspectRatio() const { return _aspectRatio; }
		inline float getXMag()        const { return _xmag; }
		inline float getYMag()        const { return _ymag; }
		inline bool  isDirty()        const { return _dirty; }

		inline const Mat4f& getProjectionMatrix() const { return _projectionMatrix; }

		const Mat4f getViewMatrix(const uint p_instanceId) const {
			if (p_instanceId < _instances.size())
				return glm::lookAt(_instances[p_instanceId]->getPosition(),
					_instances[p_instanceId]->getPosition() + _instances[p_instanceId]->getFront(),
					_instances[p_instanceId]->getUp());
			return MAT4F_ID;
		}

		// ------------------------------------------------------ SETTERS ------------------------------------------------------
		void setCameraType(CAMERA_TYPE p_type) { _type = p_type; _updateProjectionMatrix(); }

		void setZNear(const float p_znear) { _znear = glm::max<float>(1e-2f, p_znear); _updateProjectionMatrix(); }
		void setZFar(const float p_zfar) { _zfar = glm::max<float>(1e-2f, p_zfar);  _updateProjectionMatrix(); }

		void setFovy(const float p_fovy) { _fovy = p_fovy;               if (_type == CAMERA_TYPE::PERSPECTIVE) _updateProjectionMatrix(); }
		void setAspectRatio(const float p_aspectRatio) { _aspectRatio = p_aspectRatio; if (_type == CAMERA_TYPE::PERSPECTIVE) _updateProjectionMatrix(); }
		void setScreenSize(const uint p_width, const uint p_height) { setAspectRatio(float(p_width) / float(p_height)); }

		void setXMag(const float p_xmag) { _xmag = glm::max<float>(1e-2f, p_xmag); if (_type == CAMERA_TYPE::ORTHOGRAPHIC) _updateProjectionMatrix(); }
		void setYMag(const float p_ymag) { _ymag = glm::max<float>(1e-2f, p_ymag); if (_type == CAMERA_TYPE::ORTHOGRAPHIC) _updateProjectionMatrix(); }

		void setDirtyFalse() { _dirty = false; }

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		CAMERA_TYPE _type;

		float _znear = 1e-2f;
		float _zfar = 1e4f;

		// --- PERSPECTIVE ---
		float _fovy = 0.5f;
		float _aspectRatio = 1.f;

		// --- ORTHOGRAPHIC ---
		float _xmag = 1.f;
		float _ymag = 1.f;

		Mat4f _projectionMatrix = MAT4F_ID;

		bool  _dirty = false;

		void _updateProjectionMatrix() {
			switch (_type) {
			case CAMERA_TYPE::PERSPECTIVE:  _projectionMatrix = glm::perspective(_fovy, _aspectRatio, _znear, _zfar); break;
			case CAMERA_TYPE::ORTHOGRAPHIC: _projectionMatrix = glm::ortho(-0.5f * _xmag, 0.5f * _xmag, -0.5f * _ymag, 0.5f * _ymag, _znear, _zfar); break;
			default: _projectionMatrix = MAT4F_ID;
			}
			_dirty = true;
		}
	};
}
