#pragma once

#include <SDL3/SDL_video.h>

#include "utils/defines.hpp"

#include "scene/objects/meshes/texture.hpp"
#include "renderer/ogl_texture.hpp"

#include "scene/objects/meshes/mesh.hpp"
#include "renderer/ogl_mesh.hpp"

#include "renderer/stages/stage_geometry_ogl.hpp"
#include "renderer/stages/stage_lighting_ogl.hpp"
#include "renderer/stages/stage_post_processing_ogl.hpp"

#include <map>

namespace M3D
{
	enum AA_TYPE { NONE, FXAA, SMAA };

	class Renderer
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Renderer(SDL_Window* p_window);
		~Renderer();

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline float getBloomPower() const { return _bloomPower; }
		inline AA_TYPE getAAType() const { return _aaType; }

		// ------------------------------------------------------ SETTERS ------------------------------------------------------
		inline void setAAType(AA_TYPE p_aa_type) { _aaType = p_aa_type; }

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void resize(const uint p_width, const uint p_height);
		void drawFrame();

		void createMesh(Mesh* p_mesh);
		void createTexture(Texture* p_texture);

		void addInstanceMesh(Mesh* p_mesh, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix);
		void updateInstanceMesh(Mesh* p_mesh, uint p_id, const Mat4f& p_M_matrix, const Mat4f& p_V_matrix, const Mat4f& p_P_matrix);

		void deleteMesh(Mesh* p_mesh);
		void deleteTexture(Texture* p_texture);

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		StageGeometryOGL* _stageGeometryOGL;
		StageLightingOGL* _stageLightingOGL;
		StagePostProcessingOGL* _stagePostProcessingOGL;

		std::map<Mesh*, MeshOGL*> _meshes;
		std::map<Texture*, TextureOGL*> _textures;

		float _bloomPower = 0.04f;
		AA_TYPE _aaType = AA_TYPE::FXAA;
	};
}
