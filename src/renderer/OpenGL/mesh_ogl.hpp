#ifndef __MESH_OGL_HPP__
#define __MESH_OGL_HPP__

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
				glCreateBuffers(1, &_ssbo_transform_matrix);

				_vaoPrimitives.reserve(p_mesh->getPrimitives().size());
				_vboPrimitives.reserve(p_mesh->getPrimitives().size());
				_eboPrimitives.reserve(p_mesh->getPrimitives().size());

				glCreateVertexArrays((GLsizei)p_mesh->getPrimitives().size(), _vaoPrimitives.data());
				glCreateBuffers((GLsizei)p_mesh->getPrimitives().size(), _vboPrimitives.data());
				glCreateBuffers((GLsizei)p_mesh->getPrimitives().size(), _eboPrimitives.data());

				for (int i=0; i<p_mesh->getPrimitives().size() ;i++) {
					glVertexArrayVertexBuffer(_vaoPrimitives[i], 0, _vboPrimitives[i], 0, sizeof(Scene::Vertex));
					_bindValue(_vaoPrimitives[i], 0, 3, offsetof(Scene::Vertex, _position), 0);
					_bindValue(_vaoPrimitives[i], 1, 3, offsetof(Scene::Vertex, _normal), 0);
					_bindValue(_vaoPrimitives[i], 2, 2, offsetof(Scene::Vertex, _uv), 0);
					_bindValue(_vaoPrimitives[i], 3, 3, offsetof(Scene::Vertex, _tangent), 0);
					_bindValue(_vaoPrimitives[i], 4, 3, offsetof(Scene::Vertex, _bitangent), 0);

					glNamedBufferData(_vboPrimitives[i], p_mesh->getPrimitives()[i]->getVertices().size() * sizeof(Scene::Vertex), p_mesh->getPrimitives()[i]->getVertices().data(), GL_STATIC_DRAW);
					glNamedBufferData(_eboPrimitives[i], p_mesh->getPrimitives()[i]->getIndices().size() * sizeof(unsigned int), p_mesh->getPrimitives()[i]->getIndices().data(), GL_STATIC_DRAW);
					
					glVertexArrayElementBuffer(_vaoPrimitives[i], _eboPrimitives[i]);
				}
			}

			~MeshOGL() {
				for (int i=0; i<_vaoPrimitives.size() ;i++) {
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 0);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 1);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 2);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 3);
					glDisableVertexArrayAttrib(_vaoPrimitives[i], 4);
				}
				glDeleteVertexArrays((GLsizei)_vaoPrimitives.size(), _vaoPrimitives.data());

				glDeleteBuffers((GLsizei)_vboPrimitives.size(), _vboPrimitives.data());
				glDeleteBuffers((GLsizei)_eboPrimitives.size(), _eboPrimitives.data());
				glDeleteBuffers(1, &_ssbo_transform_matrix);
			}

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void bind(unsigned int p_i) {
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _ssbo_transform_matrix);
				glBindVertexArray(_vaoPrimitives[p_i]);
			}

			void addInstance(Mat4f p_M_matrix, Mat4f p_V_matrix, Mat4f p_P_matrix) {
				glDeleteBuffers(1, &_ssbo_transform_matrix);
				
				_instance_transformation.push_back(p_M_matrix);
				_instance_transformation.push_back(p_P_matrix * p_V_matrix * p_M_matrix);
				_instance_transformation.push_back(glm::transpose(glm::inverse(p_M_matrix)));

				glCreateBuffers(1, &_ssbo_transform_matrix);
				glNamedBufferStorage(_ssbo_transform_matrix, _instance_transformation.size() * sizeof(Mat4f), _instance_transformation.data(), GL_DYNAMIC_STORAGE_BIT);
			}

			void updateTransformMatrix(unsigned int p_id, Mat4f p_M_matrix, Mat4f p_V_matrix, Mat4f p_P_matrix) {
				_instance_transformation[p_id] = p_M_matrix;
				_instance_transformation[p_id + 1] = p_P_matrix * p_V_matrix * p_M_matrix;
				_instance_transformation[p_id + 2] = glm::transpose(glm::inverse(p_M_matrix));

				glNamedBufferSubData(_ssbo_transform_matrix, p_id * 3 * sizeof(Mat4f), sizeof(Mat4f) * 3, &_instance_transformation[p_id]);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint			   _ssbo_transform_matrix;
			std::vector<Mat4f> _instance_transformation;
			
			std::vector<GLuint> _vaoPrimitives;
			std::vector<GLuint> _vboPrimitives;
			std::vector<GLuint> _eboPrimitives;

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void _bindValue(GLuint p_vao, GLuint p_id, GLint p_size, GLuint p_offset, unsigned int p_updateFrequency) {
				glEnableVertexArrayAttrib(p_vao, p_id);
				glVertexArrayAttribFormat(p_vao, p_id, p_size, GL_FLOAT, GL_FALSE, p_offset);
				glVertexArrayAttribBinding(p_vao, p_id, 0);
				glVertexArrayBindingDivisor(p_vao, p_id, p_updateFrequency);
			}
		};
	}
}
#endif