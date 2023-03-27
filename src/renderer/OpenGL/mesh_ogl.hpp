#ifndef __MESH_OGL_HPP__
#define __Mesh_OGL_HPP__

#include "scene/objects/meshes/mesh.hpp"
#include "scene/objects/meshes/primitive.hpp"
#include "scene/objects/meshes/vertex.hpp"

#include "GL/gl3w.h"

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


				for (Scene::Primitive* p : p_mesh->getPrimitives()) {
					GLuint vao, vbo, ebo;

					// todo les creer tous d'un coup
					glCreateVertexArrays(1, &vao);
					glCreateBuffers(1, &vbo);
					glCreateBuffers(1, &ebo);

					glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Scene::Vertex));
					_bindValue(vao, 0, 3, offsetof(Scene::Vertex, _position), 0);
					_bindValue(vao, 1, 3, offsetof(Scene::Vertex, _normal), 0);
					_bindValue(vao, 2, 2, offsetof(Scene::Vertex, _uv), 0);
					_bindValue(vao, 3, 3, offsetof(Scene::Vertex, _tangent), 0);
					_bindValue(vao, 4, 3, offsetof(Scene::Vertex, _bitangent), 0);

					glNamedBufferData(vbo, p->getVertices().size() * sizeof(Scene::Vertex), p->getVertices().data(), GL_STATIC_DRAW);
					glNamedBufferData(ebo, p->getIndices().size() * sizeof(unsigned int), p->getIndices().data(), GL_STATIC_DRAW);
					
					glVertexArrayElementBuffer(vao, ebo);

					_vaoPrimitives.push_back(vao);
					_vboPrimitives.push_back(vbo);
					_eboPrimitives.push_back(ebo);
				}
			}

			~MeshOGL() {
				//todo delete tous d'un coup
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
				glDeleteBuffers(1, &_ssbo_transform_matrix);
			}

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void bind(unsigned int p_i) {
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, _ssbo_transform_matrix);
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
			void _bindValue(GLuint p_vao, GLuint p_id, GLint p_size, GLuint offset, unsigned int p_updateFrequency) {
				glEnableVertexArrayAttrib(p_vao, p_id);
				glVertexArrayAttribFormat(p_vao, p_id, p_size, GL_FLOAT, GL_FALSE, offset);
				glVertexArrayAttribBinding(p_vao, p_id, 0);
				glVertexArrayBindingDivisor(p_vao, p_id, p_updateFrequency);
			}
		};
	}
}
#endif
