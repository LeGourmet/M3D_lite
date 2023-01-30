#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "utils/define.hpp"

#include "glm/gtc/quaternion.hpp"
#include "glm/detail/type_quat.hpp"

namespace M3D
{
namespace Scene
{
    class Camera
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
        Camera() { _updateRotation(); }
        ~Camera() {}

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        const Quatf &getRotation() const { return _rotation; }
        const Vec3f &getPosition() const { return _position; }

        const Mat4f &getViewMatrix() const { return _viewMatrix; }
        const Mat4f &getProjectionMatrix() const { return _projectionMatrix; }

        const Vec3f getFront() { return _front; }
        const Vec3f getLeft() { return _left; }
        const Vec3f getUp() { return _up; }

        // ----------------------------------------------------- SETTERS -------------------------------------------------------
        void setScreenSize(const unsigned int p_width, const unsigned int p_height) {
            _screenWidth = p_width;
            _screenHeight = p_height;
            _aspectRatio = float(_screenWidth) / float(_screenHeight);
            _updateProjectionMatrix();
        }

        void setPosition(const Vec3f &p_position) {
            _position = p_position;
            _updateViewMatrix();
        }

        void setRotation(const Quatf &p_rotation) {
            _rotation = glm::normalize(p_rotation);
            _updateRotation();
        }

        void setNear(const float p_near) {
            _near = glm::max(1e-2f, p_near);
            _updateProjectionMatrix();
        }
        void setFar(const float p_far) {
            _far = glm::max(1e-2f, p_far);
            _updateProjectionMatrix();
        }

        void setFov(const float p_fov){
            _fov = p_fov;
            _updateProjectionMatrix();
        }

        // ----------------------------------------------------- FONCTIONS -------------------------------------------------------
        void move(const Vec3f &p_delta) {
            _position += _left * p_delta.x;
            _position += _up * p_delta.y;
            _position += _front * p_delta.z;
            _updateViewMatrix();
        }

        void rotateArround(const Vec3f& p_pivot, const Vec3f& p_delta){
            _position = Quatf(p_delta) * (_position - p_pivot) + p_pivot;
            lookAt(p_pivot);
        }
        
        void rotate(const Vec3f& p_delta) {
            _rotation *= Quatf(p_delta);
            _updateRotation();
        }

        // ne marche pas 
        void lookAt(const Vec3f& p_lookAt) {
            Vec3f dir = glm::normalize(p_lookAt - _position);
            float cosAngle = glm::dot(dir, _front);

            if (glm::abs(cosAngle+1.) < 0.001) _rotation = Quatf(PIf, {_up});
            else if(glm::abs(cosAngle-1.) < 0.001) _rotation = Quatf(1.0, { 0.0, 0.0, 0.0 });//QUAT_ID;

            float halfAngle = (float)(glm::acos(cosAngle) * 0.5);
            _rotation = Quatf(glm::cos(halfAngle), { glm::normalize(glm::cross(dir, _front)) * glm::sin(halfAngle) });
            _updateRotation();
        }

    private:
        // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
        unsigned int _screenWidth = 1u;
        unsigned int _screenHeight = 1u;
        float _aspectRatio = 1.f;
        float _near = 0.01f;
        float _far = 1e4f;
        float _fov = 60.f;

        Vec3f _position = VEC3F_ZERO;
        Quatf _rotation = Quatf(1.0, { 0.0, 0.0, 0.0 });//QUAT_ID;

        Vec3f _front = VEC3F_Z;
        Vec3f _left = VEC3F_X;
        Vec3f _up = VEC3F_Y;

        Mat4f _viewMatrix = MAT4F_ID;
        Mat4f _projectionMatrix = MAT4F_ID;

        // ----------------------------------------------------- FONCTIONS -------------------------------------------------------
        void _updateViewMatrix() { _viewMatrix = glm::lookAt(_position, _position + _front, _up); }
        void _updateProjectionMatrix() { _projectionMatrix = glm::perspective( glm::radians(_fov), _aspectRatio, _near, _far); }
        void _updateRotation() {
            Mat3f rotation = glm::mat3_cast(_rotation);
            
            // trop sensible mais marche sauf quand on regarde en up
            _front = rotation * VEC3F_Z;
            _left = glm::normalize(glm::cross(VEC3F_Y, _front));
            _up = glm::normalize(glm::cross(_front, _left));

            // induit bug => tourne sur lui même
            /*_front = rotation * VEC3F_Z;
            _left = rotation * VEC3F_X;
            _up = rotation * VEC3F_Y;*/

            _updateViewMatrix();
        }

    };
}
}
#endif
