#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "renderer/renderer.hpp"

#include <map>

namespace M3D
{
	namespace Renderer
	{
		class MeshOGL;
		class TextureOGL;
		class StageMeshOpaqueOGL;
		class StageLightingOGL;
		class StagePostProcessingOGL;
	}

	namespace Renderer
	{
		class RendererOGL : public Renderer
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			RendererOGL(SDL_Window* p_window);
			~RendererOGL();

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(const int p_width, const int p_height)  override;
			void drawFrame() override;
			
			void createMesh(Scene::Mesh* p_mesh) override;
			void createTexture(Texture* p_texture) override;

			void addInstanceMesh(Scene::Mesh* p_mesh, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) override;
			void updateInstanceMesh(Scene::Mesh* p_mesh, unsigned int p_id, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix) override;

			void deleteMesh(Scene::Mesh* p_mesh) override;
			void deleteTexture(Texture* p_texture) override;

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			StageMeshOpaqueOGL*		_stageMeshOpaqueOGL;
			StageLightingOGL*		_stageLightingOGL;
			StagePostProcessingOGL* _stagePostProcessingOGL;

			std::map<Scene::Mesh*, MeshOGL*> _meshes;
			std::map<Texture*, TextureOGL*> _textures;
		};
	}
}
#endif