#ifndef __VAO_OGL_HPP__
#define __VAO_OGL_HPP__

#include "GL/gl3w.h"
#include "utils/define.hpp"

namespace M3D
{
	namespace Renderer
	{
		class VAO_OGL
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			VAO_OGL(const std::vector<Vertex> p_vertices, const std::vector<unsigned int> p_indices) {
				glCreateBuffers(1, &_ebo);
				glNamedBufferData(_ebo, p_indices.size() * sizeof(unsigned int), p_indices.data(), GL_STATIC_DRAW);

				glCreateBuffers(1, &_vbo);
				glNamedBufferData(_vbo, p_vertices.size() * sizeof(Vertex), p_vertices.data(), GL_STATIC_DRAW);

				glCreateVertexArrays(1, &_vao);
				glVertexArrayElementBuffer(_vao, _ebo);
				_bindValue(0, 3, offsetof(Vertex, _position));
				_bindValue(1, 3, offsetof(Vertex, _normal));
				_bindValue(2, 2, offsetof(Vertex, _uvs));
				_bindValue(3, 3, offsetof(Vertex, _tangent));
				_bindValue(4, 3, offsetof(Vertex, _bitangent)); 
				glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(Vertex));
			}
			~VAO_OGL() {
				glDisableVertexArrayAttrib(_vao, 0);
				glDisableVertexArrayAttrib(_vao, 1);
				glDisableVertexArrayAttrib(_vao, 2);
				glDisableVertexArrayAttrib(_vao, 3);
				glDisableVertexArrayAttrib(_vao, 4);
				glDeleteVertexArrays(1, &_vao);
				glDeleteBuffers(1, &_vbo);
				glDeleteBuffers(1, &_ebo);
			}

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			unsigned int getId() { return (unsigned int) _vao; }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _vao = GL_INVALID_INDEX;
			GLuint _vbo = GL_INVALID_INDEX;
			GLuint _ebo = GL_INVALID_INDEX;

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void _bindValue(unsigned int p_id, unsigned int p_size, GLuint offset){
				glEnableVertexArrayAttrib(_vao, p_id);
				glVertexArrayAttribFormat(_vao, p_id, p_size, GL_FLOAT, GL_FALSE, offset);
				glVertexArrayAttribBinding(_vao, p_id, 0);
			}
		};
	}
}
#endif
