#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <SDL_video.h>

#include "utils/define.hpp"
#include "scene/objects/meshes/texture.hpp"

#include "scene/objects/meshes/mesh.hpp"

namespace M3D
{
    enum AA_TYPE { NONE, FXAA, SMAA };

    namespace Renderer
    {
        class Renderer
        {
        public:
            // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
            Renderer() {}
            //virtual Renderer(SDL_Window* p_window) = 0;
            virtual ~Renderer() = default;

            // ------------------------------------------------------ GETTERS ------------------------------------------------------
            inline float getBloomPower() { return _bloomPower; }
            inline AA_TYPE getAAType() { return _aaType; }

            inline void setAAType(AA_TYPE p_aa_type) { _aaType = p_aa_type; }

            // ----------------------------------------------------- FONCTIONS -----------------------------------------------------
            virtual void resize(const int p_width, const int p_height) = 0;
            virtual void drawFrame() = 0;

            virtual void createMesh(Scene::Mesh* p_mesh) = 0;
            virtual void createTexture(Scene::Texture* p_texture) = 0;

            virtual void addInstanceMesh(Scene::Mesh* p_mesh, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) = 0;
            virtual void updateInstanceMesh(Scene::Mesh* p_mesh, unsigned int p_id, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) = 0;

            virtual void deleteMesh(Scene::Mesh* p_mesh) = 0;
            virtual void deleteTexture(Scene::Texture* p_texture) = 0;

        protected:
            float _bloomPower = 0.04f;
            AA_TYPE _aaType = AA_TYPE::FXAA;
        };
    }
}

#endif
