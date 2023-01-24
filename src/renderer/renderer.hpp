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

            virtual void createMesh(Scene::MeshTriangle* p_mesh) = 0;
            virtual void createAmbiantMap(std::string p_path, Scene::MeshTriangle* p_mesh) = 0;
            virtual void createDiffuseMap(std::string p_path, Scene::MeshTriangle* p_mesh) = 0; 
            virtual void createSpecularMap(std::string p_path, Scene::MeshTriangle* p_mesh) = 0;
            virtual void createShininessMap(std::string p_path, Scene::MeshTriangle* p_mesh) = 0;
            virtual void createNormalMap(std::string p_path, Scene::MeshTriangle* p_mesh) = 0;
            virtual void createVAO(const std::vector<Vertex> p_vertices, const std::vector<unsigned int> p_indices, Scene::MeshTriangle* p_mesh) = 0;
            virtual void deleteMesh(Scene::MeshTriangle* p_mesh) = 0;

            void updateVSync(bool p_vSync) { _vSync = p_vSync; SDL_GL_SetSwapInterval(p_vSync); }

        protected:
            bool _vSync = true;
            Vec4f _clearColor = Vec4f(0.5, 0.5, 0.5, 1.);
        };
    }
}
#endif