#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "GL/gl3w.h"
#include "../renderer.hpp"

namespace M3D
{
	namespace Renderer
	{
		class RendererOGL : public Renderer
		{
		public:
			RendererOGL() : Renderer() {}
			~RendererOGL() {}

			void init(SDL_Window* p_window) override;
			void resize(const int p_width, const int p_height) { glViewport(0, 0, p_width, p_height); };
			void drawFrame() override;
			Texture* createTexture(const std::string p_path) override;


			/*
				void TriangleMeshModel::cleanGL()
					{
						for ( size_t i = 0; i < _meshes.size(); i++ )
						{
							_meshes[ i ].cleanGL();
						}
						for ( size_t i = 0; i < _loadedTextures.size(); i++ )
						{
							glDeleteTextures( 1, &( _loadedTextures[ i ]._id ) );
						}
						_loadedTextures.clear();
					}
			
				void TriangleMesh::cleanGL()
					{
						glDisableVertexArrayAttrib( _vao, 0 );
						glDisableVertexArrayAttrib( _vao, 1 );
						glDisableVertexArrayAttrib( _vao, 2 );
						glDisableVertexArrayAttrib( _vao, 3 );
						glDisableVertexArrayAttrib( _vao, 4 );
						glDeleteVertexArrays( 1, &_vao );
						glDeleteBuffers( 1, &_vbo );
						glDeleteBuffers( 1, &_ebo );
						glDeleteTextures( 1, &( _material._ambientMap._id ) );
						glDeleteTextures( 1, &( _material._diffuseMap._id ) );
						glDeleteTextures( 1, &( _material._specularMap._id ) );
						glDeleteTextures( 1, &( _material._shininessMap._id ) );
						glDeleteTextures( 1, &( _material._normalMap._id ) );
					}
			*/

		private:
			bool _initProgram();

		private:
			GLuint _program = GL_INVALID_INDEX;
			GLint  _uMVPMatrixLoc = GL_INVALID_INDEX;
			GLint  _uCamPosLoc = GL_INVALID_INDEX;
		};
	}
}
#endif