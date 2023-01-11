#ifndef __CONTROLLER_FREEFLY_CAMERA__
#define __CONTROLLER_FREEFLY_CAMERA__

#include "camera.hpp"
#include "keyboard_controller.hpp"
#include "mouse_controller.hpp"

namespace TutoVulkan
{
namespace Controller
{
    class FreeflyCamera : public KeyboardController, public MouseController, public Camera
    {
    public:
        FreeflyCamera() = default;

        ~FreeflyCamera() override = default;

        void receiveEvent(const SDL_Event &p_event) override;

        void clearEvents() override;

        void update(const double &p_deltaTime) override;

        void reset() override;
    };
} // namespace Controller
} // namespace TutoVulkan
#endif
