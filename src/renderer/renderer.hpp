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

            // ----------------------------------------------------- GETTERS -------------------------------------------------------
            virtual SDL_WindowFlags getWindowFlag() = 0;
            bool getVSync() { return _vSync; }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            virtual void init(SDL_Window* p_window) = 0;
            virtual void resize(const int p_width, const int p_height) = 0;
            virtual void setVSync(bool p_vSync) = 0;
            virtual void drawFrame(SDL_Window* p_window) = 0;

            virtual void createMesh(Scene::MeshTriangle* p_mesh) = 0;
            virtual void deleteMesh(Scene::MeshTriangle* p_mesh) = 0;

        protected:
            bool _vSync = true;
            float _gamma = 2.2f;
            Vec4f _clearColor = Vec4f(0., 0., 0., 0.);
        };
    }
}
#endif