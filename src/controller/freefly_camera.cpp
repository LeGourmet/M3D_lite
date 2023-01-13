#include "freefly_camera.hpp"

#include <SDL_scancode.h>

namespace TutoVulkan
{
namespace Controller
{
    void FreeflyCamera::receiveEvent(const SDL_Event &p_event)
    {
        KeyboardController::receiveEvent(p_event);
        MouseController::receiveEvent(p_event);
    }

    void FreeflyCamera::clearEvents()
    {
        KeyboardController::clearEvents();
        MouseController::clearEvents();
    }

    void FreeflyCamera::update(const double &p_deltaTime)
    {
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

    void FreeflyCamera::reset()
    {
        set(Vec3f(0.0, 0.0, -10.0), Vec3f(0.0, 0.0, 0.0));
        clearEvents();
    }

} // namespace Controller
} // namespace TutoVulkan
