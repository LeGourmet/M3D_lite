#ifndef __STAGE_MESH_TRANSPARENT_OGL_HPP__
#define __STAGE_MESH_TRANSPARENT_OGL_HPP__

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"

#include "stage_ogl.hpp"

#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class StageMeshTransparent : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StageMeshTransparent() { }

			~StageMeshTransparent() { }

			// ----------------------------------------------------- GETTERS -------------------------------------------------------

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) {
			
			}

			void execute(std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Image*, TextureOGL*> p_textures) {

			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		};
	}
}
#endif