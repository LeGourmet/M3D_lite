#include "window.hpp"

#include <SDL_init.h>
#include <SDL_error.h>
#include <SDL_timer.h>

#include "application.hpp"
#include "scene/scene_manager.hpp"
#include "user_interface/graphical_user_interface.hpp"

#include <iostream>

namespace M3D
{
    namespace InputOutput
    {
        Window::Window(Uint32 p_rendererTypeFlag) : _rendererType(p_rendererTypeFlag) {
            try {
                if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
                    throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));
                
                switch(p_rendererTypeFlag){
                    case SDL_WINDOW_OPENGL:
                        {
                            // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); // IDK
                            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
                            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
                            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

                            _window = SDL_CreateWindow(
                                Application::getInstance().getTitle().c_str(),
                                Application::getInstance().getWidth(),
                                Application::getInstance().getHeight(),
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

                            _glContext = SDL_GL_CreateContext(_window);
                            if (_glContext == nullptr) 
                                throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

                            SDL_GL_MakeCurrent(_window, _glContext);
                            break;
                        }
                    case SDL_WINDOW_VULKAN: break;

                    default : throw std::runtime_error("Exception caught: Renderer type unknown.");
                }
                
                if (_window == nullptr)
                    throw std::runtime_error("Exception caught: " + std::string(SDL_GetError()));

                setVSync(_vSync);
            }
            catch (const std::exception& p_e) {
                std::cerr << "Exception caught: " << std::endl << p_e.what() << std::endl;
                _dispose();
            }

        }
        
        Window::~Window() { _dispose(); }

        void Window::setVSync(bool p_vSync) {
            _vSync = p_vSync;
            switch (_rendererType) {
            case SDL_WINDOW_OPENGL: SDL_GL_SetSwapInterval(p_vSync); break;
            case SDL_WINDOW_VULKAN: break;
            }
        }

        void Window::setTargetFPS(unsigned int p_targetFPS) { _targetFPS = p_targetFPS; }

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
                        if (Application::getInstance().getGraphicalUserInterface().captureEvent(event)) { Application::getInstance().getSceneManager().clearEvents(); break; }
                        Application::getInstance().getSceneManager().captureEvent(event); break;
                }
            }
        }

        void Window::capFPS(float p_elapsedTime) {
            switch (_rendererType) {
                case SDL_WINDOW_OPENGL: SDL_GL_SwapWindow(_window); break;
                case SDL_WINDOW_VULKAN: break;
            }

            if (!_vSync && _targetFPS != 0) SDL_Delay((unsigned int)glm::round(glm::max<float>(0.f,1000.f/(float)_targetFPS-p_elapsedTime-1.5f)));
        }

        bool Window::_captureEvent(const SDL_Event& p_event) {
            bool keyUp = (p_event.type == SDL_EVENT_KEY_UP);
            bool keyDown = (p_event.type == SDL_EVENT_KEY_DOWN);

            if ((keyUp || keyDown) && (p_event.key.keysym.scancode >= SDL_SCANCODE_F1 && p_event.key.keysym.scancode <= SDL_SCANCODE_PRINTSCREEN)) {
                if (keyDown)
                    switch (p_event.key.keysym.scancode) {
                    case SDL_SCANCODE_F1: _switchFullScreenToMaximized(); break;
                    case SDL_SCANCODE_F2: Application::getInstance().getGraphicalUserInterface().toggleDisplayMode(); break;
                    //case SDL_SCANCODE_F3+F4+F5: take video
                    case SDL_SCANCODE_PRINTSCREEN: _takeScreenShot(); break;
                    default: break;
                    }

                return true;
            }

            return false;
        }

        void Window::_switchFullScreenToMaximized() {
            Application::getInstance().pause();
            if (SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN) { 
                SDL_SetWindowFullscreen(_window, SDL_FALSE); 
                SDL_MaximizeWindow(_window); 
            }else {SDL_SetWindowFullscreen(_window, SDL_TRUE);}
            Application::getInstance().resume();
        }

        void Window::_takeScreenShot() {
            /*pfd::save_file saveFileDialog = pfd::save_file("Save screenshot", pfd::path::home(), {".png", "*.png", "All Files", "*"});
            std::filesystem::path path = saveFileDialog.result();
            if (!path.empty()) {
                std::vector<unsigned char> pixels = std::vector<unsigned char>(_width * _height * 3);
                glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
                //stbi_flip_vertically_on_write(true);
                //stbi_write_png((path.string() + ".png").c_str(), _width, _height, 3, pixels.data(), 0);
                //stbi_flip_vertically_on_write(false);
            }*/
        }

        void Window::_dispose() {
            if (_glContext) SDL_GL_DeleteContext(_glContext);
            if (_window) SDL_DestroyWindow(_window);
            SDL_Quit();
        }
    }
}