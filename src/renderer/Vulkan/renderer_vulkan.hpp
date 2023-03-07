#ifndef __RENDERER_VULKAN_HPP__
#define __RENDERER_VULKAN_HPP__

#include "renderer/renderer.hpp"
#include "mesh_vulkan.hpp"

#include <map>

namespace M3D
{
    namespace Renderer
    {
        class RendererVulkan : Renderer
        {

        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            RendererVulkan() {}
            ~RendererVulkan() {}

            // ----------------------------------------------------- GETTERS -------------------------------------------------------
            SDL_WindowFlags getWindowFlag() { return SDL_WINDOW_VULKAN; }

            // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
            void init(SDL_Window* p_window) override { }
            void setVSync(bool p_vSync) override { _vSync = p_vSync; /*todo*/ }
            void resize(const int p_width, const int p_height)  override { /*todo*/ }
            void drawFrame(SDL_Window* p_window) override {}

            void createMesh(Scene::Mesh* p_mesh) override { _meshes.insert(std::pair<Scene::Mesh*, MeshVulkan>(p_mesh, MeshVulkan(p_mesh))); }
            void deleteMesh(Scene::Mesh* p_mesh) override { _meshes.erase(_meshes.find(p_mesh)); }

        private:
            std::map<Scene::Mesh*, MeshVulkan> _meshes;
        };
    }
}

#endif