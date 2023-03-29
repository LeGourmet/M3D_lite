#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "utils/define.hpp"
#include "utils/image.hpp"

#include <SDL_video.h>

namespace M3D
{
    namespace Scene{
        class mesh;
    }

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

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            virtual void init(SDL_Window* p_window) = 0;
            virtual void resize(const int p_width, const int p_height) = 0;
            virtual void drawFrame(SDL_Window* p_window) = 0;

            virtual void createMesh(Scene::Mesh* p_mesh) = 0;
            virtual void createTexture(Image* p_texture) = 0;

            virtual void addInstance(Scene::Mesh* p_mesh, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) = 0;
            virtual void updateInstance(Scene::Mesh* p_mesh, unsigned int p_id, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) = 0;

            virtual void deleteMesh(Scene::Mesh* p_mesh) = 0;
            virtual void deleteTexture(Image* p_texture) = 0;

        protected:
            float _gamma = 2.2f;
            Vec4f _clearColor = Vec4f(0., 0., 0., 0.);
        };
    }
}
#endif