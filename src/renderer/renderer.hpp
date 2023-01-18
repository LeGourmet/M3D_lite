#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <SDL.h>

#include "scene/vertex.hpp"
#include "texture.hpp"
#include "object.hpp"

#include <string>

namespace M3D
{
    namespace Renderer
    {
        class Renderer
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Renderer() {}
            virtual ~Renderer() = default;

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            virtual void init(SDL_Window* p_window) = 0;
            virtual void resize(const int p_width, const int p_height) = 0;
            virtual void drawFrame() = 0;
            virtual Texture* createTexture(const std::string p_path) = 0;
            virtual Object* createObject(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices ) = 0;
        };
    }
}
#endif