#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <SDL.h>

#include "utils/define.hpp"
#include "scene/mesh_triangle.hpp"

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
            virtual void drawFrame(SDL_Window* p_window) = 0;
            virtual void createTexture(const std::string p_path, Scene::MeshTriangle* mesh) = 0;
            virtual void deleteTexture(unsigned int p_id, Scene::MeshTriangle* mesh) = 0;
            virtual void createVAO(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, Scene::MeshTriangle* mesh) = 0;
            virtual void deleteVAO(unsigned int p_id, Scene::MeshTriangle* mesh) = 0;

            bool _vSync = true;
            Vec4f _clearColor = Vec4f(0.5, 0.5, 0.5, 1.);
        };
    }
}
#endif