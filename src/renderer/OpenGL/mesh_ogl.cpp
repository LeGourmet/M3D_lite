#include "mesh_ogl.hpp"

#include "utils/define.hpp"
#include "utils/image.hpp"

#include <string>
#include <cmath>
#include <algorithm>

namespace M3D
{
	namespace Renderer
	{
		MeshOGL::MeshOGL(Scene::MeshTriangle* p_mesh) {
			glCreateVertexArrays(1, &_vao);

			glCreateBuffers(1, &_vbo);
			glNamedBufferData(_vbo, p_mesh->getVertices().size() * sizeof(Vertex), p_mesh->getVertices().data(), GL_STATIC_DRAW);
			_bindValue(0, 3, offsetof(Vertex, _position));
			_bindValue(1, 3, offsetof(Vertex, _normal));
			_bindValue(2, 2, offsetof(Vertex, _uv));
			_bindValue(3, 3, offsetof(Vertex, _tangent));
			_bindValue(4, 3, offsetof(Vertex, _bitangent));
			glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(Vertex));

			glCreateBuffers(1, &_ebo);
			glNamedBufferData(_ebo, p_mesh->getIndices().size() * sizeof(unsigned int), p_mesh->getIndices().data(), GL_STATIC_DRAW);
			glVertexArrayElementBuffer(_vao, _ebo);

			if (p_mesh->_hasAmbientMap)   _generateTexture(p_mesh->_ambientMapPath, &_idAmbientMap);
			if (p_mesh->_hasDiffuseMap)   _generateTexture(p_mesh->_diffuseMapPath, &_idDiffuseMap);
			if (p_mesh->_hasSpecularMap)  _generateTexture(p_mesh->_specularMapPath, &_idSpecularMap);
			if (p_mesh->_hasShininessMap) _generateTexture(p_mesh->_shininessMapPath, &_idShininessMap);
			if (p_mesh->_hasNormalMap)    _generateTexture(p_mesh->_normalMapPath, &_idNormalMap);
		}

		MeshOGL::~MeshOGL() {
			glDisableVertexArrayAttrib(_vao, 0);
			glDisableVertexArrayAttrib(_vao, 1);
			glDisableVertexArrayAttrib(_vao, 2);
			glDisableVertexArrayAttrib(_vao, 3);
			glDisableVertexArrayAttrib(_vao, 4);
			glDeleteVertexArrays(1, &_vao);
			glDeleteBuffers(1, &_vbo);
			glDeleteBuffers(1, &_ebo);

			glDeleteTextures(1, &_idAmbientMap);
			glDeleteTextures(1, &_idDiffuseMap);
			glDeleteTextures(1, &_idSpecularMap);
			glDeleteTextures(1, &_idShininessMap);
			glDeleteTextures(1, &_idNormalMap);
		}

		void MeshOGL::_bindValue(GLuint p_id, GLint p_size, GLuint offset) {
			glEnableVertexArrayAttrib(_vao, p_id);
			glVertexArrayAttribFormat(_vao, p_id, p_size, GL_FLOAT, GL_FALSE, offset);
			glVertexArrayAttribBinding(_vao, p_id, 0);
		}

		void MeshOGL::_generateTexture(std::string p_texPath, GLuint* p_textureId) {
			int width, height, nbChannels;
			unsigned char* pixels = Image::load(p_texPath, &width, &height, &nbChannels);

			if (pixels != nullptr) {
				glCreateTextures(GL_TEXTURE_2D, 1, p_textureId);

				GLenum format, internalFormat;
				switch (nbChannels) {
				case 1: format = GL_RED; internalFormat = GL_R32F; break;
				case 2: format = GL_RG; internalFormat = GL_RG32F; break;
				case 3: format = GL_RGB; internalFormat = GL_RGB32F; break;
				default: format = GL_RGBA; internalFormat = GL_RGBA32F; break;
				}

				unsigned int lvMipMap = (unsigned int)std::floor(std::log2(std::max<int>(width, height))) + 1;
				glTextureStorage2D(*p_textureId, lvMipMap, internalFormat, width, height);
				glTextureParameteri(*p_textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(*p_textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTextureParameteri(*p_textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTextureParameteri(*p_textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTextureSubImage2D(*p_textureId, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);
				glGenerateTextureMipmap(*p_textureId);
			}

			Image::free(pixels);
		}

	}
}