#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <SDL.h>

#include "texture.hpp"

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
            virtual Object* createObject(   std::vector<Vec3f> p_vertices, 
                                            std::vector<Vec3f> _normals, 
                                            std::vector<Vec3u> _triangles, 
                                            std::vector<Vec2f> _uvs, 
                                            std::vector<Vec3f> _tangents, 
                                            std::vector<Vec3f> _bitangents) = 0;
        };
    }
}
#endif