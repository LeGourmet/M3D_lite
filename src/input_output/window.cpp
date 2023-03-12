#include "window.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "user_interface/graphical_user_interface.hpp"

#include <SDL_init.h>
#include <SDL_error.h>
#include <SDL_timer.h>

#include <iostream>

namespace M3D
{
    namespace InputOutput
    {
        Window::Window() {
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
                throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));
        }
        Window::~Window() { _dispose(); }

        void Window::create(SDL_WindowFlags p_rendererTypeFlag) {
            try {
                _window = SDL_CreateWindow(
                    Application::getInstance().getTitle().c_str(),
                    Application::getInstance().getWidth(),
                    Application::getInstance().getHeight(),
                    p_rendererTypeFlag | SDL_WINDOW_RESIZABLE);
                if (_window == nullptr)
                    throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));
            }
            catch (const std::exception& p_e) {
                std::cerr << "Exception caught: " << std::endl << p_e.what() << std::endl;
                _dispose();
            }
        }

        void Window::captureEvents() {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_EVENT_WINDOW_MINIMIZED:
                        Application::getInstance().pause();
                        do { SDL_WaitEvent(&event); } while (!(event.type == SDL_EVENT_WINDOW_RESTORED));
                        Application::getInstance().resume();
                        break;
                    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                        Application::getInstance().resize(event.window.data1, event.window.data2);
                        break;
                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                        Application::getInstance().stop();
                        break;
                    case SDL_EVENT_WINDOW_FOCUS_LOST:
                        Application::getInstance().pause();
                        do { SDL_WaitEvent(&event); } while (!(event.type == SDL_EVENT_WINDOW_FOCUS_GAINED));
                        Application::getInstance().resume();
                        break;   
                    case SDL_EVENT_QUIT:
                        Application::getInstance().stop();
                        break;
                    default:
                        if (_captureEvent(event)) break;
                        if (Application::getInstance().getGraphicalUserInterface().captureEvent(event)) break;
                        Application::getInstance().getSceneManager().captureEvent(event); break;
                }
            }
        }

        void Window::chronoUpdate() {
            _time = SDL_GetTicks();
        }

        unsigned long long Window::getDeltaTime() {
            unsigned long long previousTime = _time;
            chronoUpdate();
            return _time - previousTime;
        }

        bool Window::_captureEvent(SDL_Event p_event) {
            bool keyUp = (p_event.type == SDL_EVENT_KEY_UP);
            bool keyDown = (p_event.type == SDL_EVENT_KEY_DOWN);

            if ((keyUp || keyDown) && (p_event.key.keysym.scancode >= SDL_SCANCODE_F1 && p_event.key.keysym.scancode <= SDL_SCANCODE_PRINTSCREEN)) {
                if (keyDown)
                    switch (p_event.key.keysym.scancode) {
                    case SDL_SCANCODE_F1: _switchFullScreen2Maximized(); break;
                    // add hide / unhide GUI
                    // add videoCapture
                    case SDL_SCANCODE_PRINTSCREEN: _takeScreenShot(); break;
                    default: break;
                    }

                return true;
            }

            return false;
        }

        void Window::_switchFullScreen2Maximized() {
            Application::getInstance().pause();
            if (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN) { 
                SDL_SetWindowFullscreen(_window, SDL_FALSE); 
                SDL_MaximizeWindow(_window); 
            }else {SDL_SetWindowFullscreen(_window, SDL_TRUE);}
            Application::getInstance().resume();
        }

        void Window::_takeScreenShot() {
            try {
                /*SDL_Surface* surface = SDL_CreateRGBSurface(0, Application::getInstance().getWidth(), Application::getInstance().getHeight(), 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
                    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif
                );
                SDL_LockSurface(surface);
                SDL_RenderReadPixels(SDL_GetRenderer(_window), NULL, surface->format->format, surface->pixels, surface->pitch);
                const long long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();
                std::string filePath = "./screenshot/screenshot_" + std::to_string(timestamp) + ".bmp";
                SDL_SaveBMP(surface, filePath.c_str()); //IMG_savePNG(surface, ". / screenshot / screenTest.png");
                SDL_UnlockSurface(surface);
                SDL_FreeSurface( surface );*/
            }
            catch (const std::exception& e) {
                std::cerr << "Cannot save screenshot: " << e.what() << std::endl;
            }
        }

        void Window::_dispose() {
            if (_window) SDL_DestroyWindow(_window);
            _window = nullptr;
            SDL_Quit();
        }
    }
}