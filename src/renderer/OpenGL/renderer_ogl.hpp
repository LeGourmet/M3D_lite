#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "renderer/renderer.hpp"

#include <map>

namespace M3D
{
	namespace Renderer {
		class MeshOGL;
		class TextureOGL;
		class GeometryPassOGL;
		class ShadingPassOGL;
		class FinalPassOGL;
	}

	namespace Renderer
	{
		class RendererOGL : public Renderer
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			RendererOGL();
			~RendererOGL();

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			inline SDL_WindowFlags getWindowFlag() { return SDL_WINDOW_OPENGL; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void init(SDL_Window* p_window) override;
			void resize(const int p_width, const int p_height)  override;
			void drawFrame(SDL_Window* p_window) override;
			
			void createMesh(Scene::Mesh* p_mesh) override;
			void createTexture(Image* p_texture) override;

			void addInstance(Scene::Mesh* p_mesh, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) override;
			void updateInstance(Scene::Mesh* p_mesh, unsigned int p_id, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) override;

			void deleteMesh(Scene::Mesh* p_mesh) override;
			void deleteTexture(Image* p_texture) override;

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			// mieux abstraire + renderGraph
			GeometryPassOGL* _geometryPass;
			ShadingPassOGL* _shadingPass;
			FinalPassOGL* _finalPass;

			std::map<Scene::Mesh*,MeshOGL*> _meshes;
			std::map<Image*, TextureOGL*> _textures;
		};
	}
}
#endif