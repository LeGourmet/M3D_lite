#ifndef __FREEFLY_CAMERA_HPP__
#define __FREEFLY_CAMERA_HPP__

#include "camera.hpp"

namespace M3D
{
namespace Controller
{
    class FreeflyCamera : public Camera
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
        FreeflyCamera() = default;
        ~FreeflyCamera() override = default;

        // ----------------------------------------------------- FONCTIONS -------------------------------------------------------

        void update(const double &p_deltaTime) override {
            // Rotation.
            if (_mouseLeftPressed)
            {
                rotate(Vec3f(0.005f * float(_deltaMousePosition.y),
                    -0.005f * float(_deltaMousePosition.x),
                    0.f));
                _deltaMousePosition.x = 0;
                _deltaMousePosition.y = 0;
            }

            // Translation.
            Vec3f translation = VEC3F_ZERO;

            if (_isKeyPressed(SDL_SCANCODE_W) || _isKeyPressed(SDL_SCANCODE_UP))
                translation.z++;
            if (_isKeyPressed(SDL_SCANCODE_S) || _isKeyPressed(SDL_SCANCODE_DOWN))
                translation.z--;
            if (_isKeyPressed(SDL_SCANCODE_A) || _isKeyPressed(SDL_SCANCODE_LEFT))
                translation.x++;
            if (_isKeyPressed(SDL_SCANCODE_D) || _isKeyPressed(SDL_SCANCODE_RIGHT))
                translation.x--;
            if (_isKeyPressed(SDL_SCANCODE_R))
                translation.y++;
            if (_isKeyPressed(SDL_SCANCODE_F))
                translation.y--;
            if (translation == VEC3F_ZERO)
                return;

            translation *= _speed;
            translation *= p_deltaTime;

            if (_isKeyPressed(SDL_SCANCODE_LSHIFT)) { translation *= 2.f; }

            if (_isKeyPressed(SDL_SCANCODE_LCTRL)) { translation /= 2.f; }

            move(translation);
        }

        void reset() override {
            set(Vec3f(0.0, 0.0, -10.0), Vec3f(0.0, 0.0, 0.0));
            clearEvents();
        }
    };
}
}
#endif
