#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <SDL_video.h>

#include "utils/define.hpp"
#include "utils/image.hpp"

#include "scene/objects/meshes/mesh.hpp"

namespace M3D
{
    namespace Renderer
    {
        class Renderer
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Renderer() {}
            //virtual Renderer(SDL_Window* p_window) = 0;
            virtual ~Renderer() = default;

            // ----------------------------------------------------- GETTERS -------------------------------------------------------
            inline float getGamma() { return _gamma; }
            inline Vec4f& getClearColor() { return _clearColor; }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            virtual void resize(const int p_width, const int p_height) = 0;
            virtual void drawFrame() = 0;

            virtual void createMesh(Scene::Mesh* p_mesh) = 0;
            virtual void createTexture(Image* p_texture) = 0;

            virtual void addInstanceMesh(Scene::Mesh* p_mesh, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) = 0;
            virtual void updateInstanceMesh(Scene::Mesh* p_mesh, unsigned int p_id, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) = 0;

            virtual void deleteMesh(Scene::Mesh* p_mesh) = 0;
            virtual void deleteTexture(Image* p_texture) = 0;

        protected:
            float _gamma = 2.2f; // 3.5 = blender
            Vec4f _clearColor = VEC4F_ZERO;
        };
    }
}
#endif