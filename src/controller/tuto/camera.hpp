#ifndef __CONTROLLER_CAMERA__
#define __CONTROLLER_CAMERA__

#include "utils/define.hpp"
#include <SDL_events.h>
#include <detail/type_quat.hpp>
#include <gtc/quaternion.hpp>

namespace TutoVulkan
{
namespace Controller
{
    class Camera
    {
    public:
        // DESTRUCTOR / CONSTRUCTOR
        Camera() { _updateRotation(); }
        virtual ~Camera() = default;

        // VIRTUAL FONCTIONS
        virtual void receiveEvent(const SDL_Event &p_event) = 0;
        virtual void clearEvents() = 0;
        virtual void update(const double &p_deltaTime) = 0;
        virtual void reset() = 0;

        // GETTERS
        const Quatf &getRotation() const { return _rotation; }
        const Vec3f &getPosition() const { return _position; }

        const Mat4f &getViewMatrix() const { return _viewMatrix; }
        const Mat4f &getProjectionMatrix() const { return _projectionMatrix; }

        const Vec3f &getFront() const { return _front; }
        const Vec3f &getLeft() const { return _left; }
        const Vec3f &getUp() const { return _up; }

        uint getScreenWidth() const { return _screenWidth; }
        uint getScreenHeight() const { return _screenHeight; }
        float getAspectRatio() const { return _aspectRatio; }

        float *getNear() { return &_near; }
        float *getFar() { return &_far; }
        float *getFov() { return &_fov; }
        float *getSpeed() { return &_speed; }

        // SETTERS
        void setScreenSize(const uint p_width, const uint p_height)
        {
            _screenWidth = p_width;
            _screenHeight = p_height;
            _aspectRatio = float(_screenWidth) / float(_screenHeight);
            _updateProjectionMatrix();
        }

        void setPosition(const Vec3f &p_position)
        {
            _position = p_position;
            _updateViewMatrix();
        }

        void setRotation(const Quatf &p_rotation)
        {
            _rotation = glm::normalize(p_rotation);
            _updateRotation();
        }

        void set(const Vec3f &p_position, const Quatf &p_rotation)
        {
            _position = p_position;
            setRotation(p_rotation);
        }

        void setNear(const float p_near)
        {
            _near = glm::max(1e-2f, p_near);
            _updateProjectionMatrix();
        }
        void setFar(const float p_far)
        {
            _far = glm::max(1e-2f, p_far);
            _updateProjectionMatrix();
        }

        void setFov(const float p_fov)
        {
            _fov = p_fov;
            _updateProjectionMatrix();
        }

        void setSpeed(const float p_speed)
        {
            _speed = p_speed;
        }

        //  FONCTIONS
        void move(const Vec3f &p_delta)
        {
            _position += _left * p_delta.x;
            _position += _up * p_delta.y;
            _position += _front * p_delta.z;
            _updateViewMatrix();
        }

        void rotate(const Vec3f &p_delta)
        {
            _rotation = _rotation * Quatf(p_delta);
            _updateRotation();
        }

    protected:
        //  ATTRIBUTS
        uint _screenWidth = 1u;
        uint _screenHeight = 1u;
        float _aspectRatio = 1.f;
        float _near = 0.01f;
        float _far = 1e4f;
        float _fov = 60.f;
        float _speed = 100.f;

        Vec3f _position = VEC3F_Z * -10.f;
        Quatf _rotation = Quatf(1.0, {0.0, 0.0, 0.0});

        Vec3f _front = VEC3F_Z;
        Vec3f _left = VEC3F_X;
        Vec3f _up = VEC3F_Y;

        Mat4f _viewMatrix{};
        Mat4f _projectionMatrix{};

        //  FONCTIONS
        void _updateRotation()
        {
            Mat3d rotation = glm::mat3_cast(_rotation);
            _front = rotation * VEC3F_Z;
            _left = glm::normalize(glm::cross(VEC3F_Y, _front));
            _up = glm::normalize(glm::cross(_front, _left));
            _updateViewMatrix();
        }

        void _updateViewMatrix()
        {
            _viewMatrix = glm::lookAt(_position, _position + _front, _up);
        }

        void _updateProjectionMatrix()
        {
            _projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, _near, _far);
        }
    };
} // namespace Controller
} // namespace TutoVulkan
#endif
