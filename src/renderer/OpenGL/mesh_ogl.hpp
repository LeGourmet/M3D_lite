#ifndef __MESH_OGL_HPP__
#define __Mesh_OGL_HPP__

#include "GL/gl3w.h"

#include "scene/mesh_triangle.hpp"

namespace M3D
{
	namespace Renderer
	{
		class MeshOGL
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			MeshOGL(Scene::MeshTriangle* p_mesh);
			~MeshOGL();

			// ------------------------------------------------------ GETTERS --------------------------------------------------------
			GLuint getIdAmbientMap() { return _idAmbientMap; }
			GLuint getIdAlbedoMap() { return _idAlbedoMap; }
			GLuint getIdMetalnessMap() { return _idMetalnessMap; }
			GLuint getIdRoughnessMap() { return _idRoughnessMap; }
			GLuint getIdNormalMap() { return _idNormalMap; }
			GLuint getVao() { return _vao; }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _idAmbientMap = 0;
			GLuint _idAlbedoMap = 0;
			GLuint _idMetalnessMap = 0;
			GLuint _idRoughnessMap = 0;
			GLuint _idNormalMap = 0;

			GLuint _vao = GL_INVALID_INDEX;
			GLuint _vbo = GL_INVALID_INDEX;
			GLuint _ebo = GL_INVALID_INDEX;
		
			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void _bindValue(GLuint p_id, GLint p_size, GLuint offset);
			void _generateTexture(std::string p_texPath, GLuint* p_textureId);
		};
	}
}
#endif
