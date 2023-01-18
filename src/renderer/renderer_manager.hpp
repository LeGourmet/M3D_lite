#ifndef __RENDERER_MANAGER_HPP__
#define __RENDERER_MANAGER_HPP__

#include "renderer.hpp"
#include "OpenGl/renderer_ogl.hpp"

namespace M3D
{
    namespace Renderer
    {
        enum RENDERER_TYPE { OPENGL, VULKAN, METAL };

        class RendererManager
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            RendererManager() {
                _renderer = new RendererOGL();
                _type = RENDERER_TYPE::OPENGL;
            }
            ~RendererManager() { delete _renderer; }

            Renderer& getRenderer() const { *_renderer; }
            SDL_WindowFlags getRendererWindowFlags() const {
                SDL_WindowFlags engineTypeFlag;
                switch (_type) {
                    case RENDERER_TYPE::OPENGL: engineTypeFlag = SDL_WINDOW_OPENGL; break;
                    case RENDERER_TYPE::VULKAN: engineTypeFlag = SDL_WINDOW_VULKAN; break;
                    case RENDERER_TYPE::METAL: engineTypeFlag = SDL_WINDOW_METAL; break;
                    default: throw std::runtime_error("Non-supported renderer by this window!"); break;
                }
                return engineTypeFlag;
            }

        private:
            RENDERER_TYPE _type;
            Renderer* _renderer;
        };
    }
}
#endif
