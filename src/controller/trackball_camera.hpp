#ifndef __TRACKBALL_CAMERA_HPP__
#define __TRACKBALL_CAMERA_HPP__

#include "camera.hpp"

namespace M3D
{
    namespace Controller
    {
        class TrackballCamera : public Camera
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
            TrackballCamera() = default;
            ~TrackballCamera() override = default;

            // ----------------------------------------------------- FONCTIONS -------------------------------------------------------

            void update(const double& p_deltaTime) override {
            }

            void reset() override {
            }
        };
    }
}
#endif
