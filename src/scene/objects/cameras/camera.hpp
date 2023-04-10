#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "glm/detail/_fixes.hpp"
#include "glm/gtx/quaternion.hpp"

#include "utils/define.hpp"
#include "scene/objects/object.hpp"

namespace M3D
{
    enum CAMERA_TYPE { PERSPECTIVE, ORTHOGRAPHIC };

namespace Scene
{
    class Camera : public Object
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
        /*
        * p_arg1 is interpreted as fovy or xmag, it depend of the CAMERA_TYPE
        * p_arg2 is interpreted as aspect ratio or ymag, it depend of the CAMERA_TYPE
        */
        Camera(float p_arg1, float p_arg2, float p_znear, float p_zfar, CAMERA_TYPE p_type) : _type(p_type) {
            switch (p_type) {
                case CAMERA_TYPE::PERSPECTIVE:  setFovy(p_arg1); setAspectRatio(p_arg2); break;
                case CAMERA_TYPE::ORTHOGRAPHIC: setXMag(p_arg1); setYMag(p_arg2);        break;
            }

            setZNear(p_znear);
            setZFar(p_zfar);
        }
        ~Camera() {}

        // ------------------------------------------------------ GETTERS ------------------------------------------------------
        const Mat4f getViewMatrix(const unsigned int p_instanceId) const {
            if (p_instanceId < _instances.size())
                return glm::lookAt(_instances[p_instanceId]->getPosition(),
                                   _instances[p_instanceId]->getPosition() + _instances[p_instanceId]->getFront(), 
                                   _instances[p_instanceId]->getUp());
            return MAT4F_ID;
        }

        const Mat4f getProjectionMatrix() const {
            switch (_type) {
                case CAMERA_TYPE::PERSPECTIVE:  return glm::perspective(_fovy, _aspectRatio, _znear, _zfar);
                case CAMERA_TYPE::ORTHOGRAPHIC: return glm::ortho(-0.5f*_xmag, 0.5f*_xmag, -0.5f*_ymag, 0.5f*_ymag, _znear, _zfar);
            }
        }

        // ------------------------------------------------------ SETTERS ------------------------------------------------------
        inline void setCameraType(CAMERA_TYPE p_type) { _type = p_type; }

        inline void setZNear(const float p_znear) { _znear = glm::max<float>(1e-2f, p_znear); }
        inline void setZFar(const float p_zfar) { _zfar = glm::max<float>(1e-2f, p_zfar); }

        inline void setFovy(const float p_fovy) { _fovy = p_fovy; }
        inline void setAspectRatio(const float p_aspectRatio) { _aspectRatio = p_aspectRatio; }
        inline void setScreenSize(const unsigned int p_width, const unsigned int p_height) { _aspectRatio = float(p_width) / float(p_height); }
        
        inline void setXMag(const float p_xmag) { _xmag = glm::max<float>(1e-2f, p_xmag); }
        inline void setYMag(const float p_ymag) { _ymag = glm::max<float>(1e-2f, p_ymag); }

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
    };
}
}
#endif
