#ifndef __MESH_OGL_HPP__
#define __Mesh_OGL_HPP__

#include "GL/gl3w.h"

#include "scene/objects/meshes/mesh.hpp"
#include "scene/objects/meshes/primitive.hpp"
#include "scene/objects/meshes/vertex.hpp"

namespace M3D
{
	namespace Renderer
	{
		class MeshOGL
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			MeshOGL(Scene::Mesh* p_mesh) {
				std::vector<Scene::Primitive*> mesh_primitives = p_mesh->getPrimitives();

				_vaoPrimitives.reserve(mesh_primitives.size());
				_vboPrimitives.reserve(mesh_primitives.size());
				_eboPrimitives.reserve(mesh_primitives.size());

				for (Scene::Primitive* p : mesh_primitives) {
					GLuint vao, vbo, ebo;
					glCreateVertexArrays(1, &vao);
					
					glCreateBuffers(1, &vbo);

					glNamedBufferData(vbo, p->getVertices().size() * sizeof(Scene::Vertex), p->getVertices().data(), GL_STATIC_DRAW);
					_bindValue(vao, 0, 3, offsetof(Scene::Vertex, _position));
					_bindValue(vao, 1, 3, offsetof(Scene::Vertex, _normal));
					_bindValue(vao, 2, 2, offsetof(Scene::Vertex, _uv));
					_bindValue(vao, 3, 3, offsetof(Scene::Vertex, _tangent));
					_bindValue(vao, 4, 3, offsetof(Scene::Vertex, _bitangent));
					glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Scene::Vertex));

					glCreateBuffers(1, &ebo);
					glNamedBufferData(ebo, p->getIndices().size() * sizeof(unsigned int), p->getIndices().data(), GL_STATIC_DRAW);
					glVertexArrayElementBuffer(vao, ebo);
				}
			}

			~MeshOGL() {
				for (int i = 0; i < _vaoPrimitives.size();i++) {
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 0);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 1);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 2);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 3);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 4);
					glDeleteVertexArrays(1, &_vaoPrimitives[i]);
					glDeleteBuffers(1, &_vboPrimitives[i]);
					glDeleteBuffers(1, &_eboPrimitives[i]);
				}
			}

			// ------------------------------------------------------ GETTERS --------------------------------------------------------
			std::vector<GLuint>& getVaos() { return _vaoPrimitives; }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			std::vector<GLuint> _vaoPrimitives;
			std::vector<GLuint> _vboPrimitives;
			std::vector<GLuint> _eboPrimitives;

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void _bindValue(GLuint p_vao, GLuint p_id, GLint p_size, GLuint offset) {
				glEnableVertexArrayAttrib(p_vao, p_id);
				glVertexArrayAttribFormat(p_vao, p_id, p_size, GL_FLOAT, GL_FALSE, offset);
				glVertexArrayAttribBinding(p_vao, p_id, 0);
			}
		};
	}
}
#endif
