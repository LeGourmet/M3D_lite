#ifndef __OBJECT_OGL_HPP__
#define __OBJECT_OGL_HPP__

#include "GL/gl3w.h"

#include "utils/image.hpp"
#include "utils/define.hpp"

#include <string>
#include <cmath>
#include <algorithm>

namespace M3D
{
	namespace Renderer
	{
		class Object_OGL
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			Object_OGL() {}
			~Object_OGL() {
				glDisableVertexArrayAttrib(_vao, 0);
				glDisableVertexArrayAttrib(_vao, 1);
				glDisableVertexArrayAttrib(_vao, 2);
				glDisableVertexArrayAttrib(_vao, 3);
				glDisableVertexArrayAttrib(_vao, 4);
				glDeleteVertexArrays(1, &_vao);
				glDeleteBuffers(1, &_vbo);
				glDeleteBuffers(1, &_ebo);

				glDeleteTextures(1, &_ambientMap);
				glDeleteTextures(1, &_diffuseMap);
				glDeleteTextures(1, &_specularMap);
				glDeleteTextures(1, &_shininessMap);
				glDeleteTextures(1, &_normalMap);
			}

			// ------------------------------------------------------ GETTERS --------------------------------------------------------
			GLuint getAmbientMap() { return _ambientMap; }
			GLuint getDiffuseMap() { return _diffuseMap; }
			GLuint getSpecularMap() { return _specularMap; }
			GLuint getShininessMap() { return _shininessMap; }
			GLuint getNormalMap() { return _normalMap; }
			GLuint getVao() { return _vao; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void generateAmbiantMap(std::string p_path) { _generateTexture(p_path,&_ambientMap); }
			void generateDiffuseMap(std::string p_path) { _generateTexture(p_path,&_diffuseMap); }
			void generateSpecularMap(std::string p_path) { _generateTexture(p_path,&_specularMap); }
			void generateShininessMap(std::string p_path) { _generateTexture(p_path,&_shininessMap); }
			void generateNormalMap(std::string p_path) { _generateTexture(p_path,&_normalMap); }
			void generateVAO(const std::vector<Vertex> p_vertices, const std::vector<unsigned int> p_indices) {
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
		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _ambientMap = 0;
			GLuint _diffuseMap = 0;
			GLuint _specularMap = 0;
			GLuint _shininessMap = 0;
			GLuint _normalMap = 0;

			GLuint _vao = GL_INVALID_INDEX;
			GLuint _vbo = GL_INVALID_INDEX;
			GLuint _ebo = GL_INVALID_INDEX;
		
			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void _bindValue(unsigned int p_id, unsigned int p_size, GLuint offset) {
				glEnableVertexArrayAttrib(_vao, p_id);
				glVertexArrayAttribFormat(_vao, p_id, p_size, GL_FLOAT, GL_FALSE, offset);
				glVertexArrayAttribBinding(_vao, p_id, 0);
			}

			void _generateTexture(std::string p_path, GLuint* p_textureId) {
				Image image;
				if (image.load(p_path)) {
					glCreateTextures(GL_TEXTURE_2D, 1, p_textureId);

					GLenum format, internalFormat;
					switch (image._nbChannels) {
					case 1: format = GL_RED; internalFormat = GL_R32F; break;
					case 2: format = GL_RG; internalFormat = GL_RG32F; break;
					case 3: format = GL_RGB; internalFormat = GL_RGB32F; break;
					default: format = GL_RGBA; internalFormat = GL_RGBA32F; break;
					}

					unsigned int lvMipMap = (unsigned int)std::floor(std::log2(std::max<int>(image._width, image._height))) + 1;
					glTextureStorage2D(*p_textureId, lvMipMap, internalFormat, image._width, image._height);
					glTextureParameteri(*p_textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTextureParameteri(*p_textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTextureParameteri(*p_textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTextureParameteri(*p_textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTextureSubImage2D(*p_textureId, 0, 0, 0, image._width, image._height, format, GL_UNSIGNED_BYTE, image._pixels);
					glGenerateTextureMipmap(*p_textureId);
				}
			}
		};
	}
}
#endif
