#ifndef __RENDERER_OGL_HPP__
#define __RENDERER_OGL_HPP__

#include "GL/gl3w.h"

#include "mesh_ogl.hpp"
#include "renderer/renderer.hpp"

#include <map>

namespace M3D
{
	namespace Renderer
	{
		class RendererOGL : public Renderer
		{
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			RendererOGL() {	}
			~RendererOGL() {
				for (std::pair<Scene::MeshTriangle*, MeshOGL*> pair : _meshes) delete pair.second;
				glDeleteProgram(_basePass);
				glDeleteProgram(_shadingPass);
				glDeleteProgram(_finalPass);
			}

			// ----------------------------------------------------- GETTERS -------------------------------------------------------
			SDL_WindowFlags getWindowFlag() { return SDL_WINDOW_OPENGL; }

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void init(SDL_Window* p_window) override {
				SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
				SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
				SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

				SDL_GLContext glContext = SDL_GL_CreateContext(p_window);
				if( glContext == nullptr ) throw std::exception(SDL_GetError());
				SDL_GL_MakeCurrent(p_window, glContext);
				setVSync(_vSync);
				if (gl3wInit()) throw std::exception("gl3wInit() failed");
				if (!gl3wIsSupported(4, 5)) throw std::exception("OpenGL version not supported");

				_basePass				= _initProgram("src/renderer/OpenGL/shaders/basePass.vert", "src/renderer/OpenGL/shaders/basePass.frag");
				_uMatrix_MVPLoc			= glGetUniformLocation(_basePass, "uMatrix_MVP");
				_uMatrix_MLoc			= glGetUniformLocation(_basePass, "uMatrix_M");
				_uMatrix_NormalLoc		= glGetUniformLocation(_basePass, "uMatrix_Normal");
				_uAmbientLoc			= glGetUniformLocation(_basePass, "uAmbient");
				_uDiffuseLoc			= glGetUniformLocation(_basePass, "uDiffuse");
				_uSpecularLoc			= glGetUniformLocation(_basePass, "uSpecular");
				_uShininessLoc			= glGetUniformLocation(_basePass, "uShininess");
				_uHasAmbientMapLoc		= glGetUniformLocation(_basePass, "uHasAmbientMap");
				_uHasDiffuseMapLoc		= glGetUniformLocation(_basePass, "uHasDiffuseMap");
				_uHasSpecularMapLoc		= glGetUniformLocation(_basePass, "uHasSpecularMap");
				_uHasShininessMapLoc	= glGetUniformLocation(_basePass, "uHasShininessMap");
				_uHasNormalMapLoc		= glGetUniformLocation(_basePass, "uHasNormalMap");
				
				_shadingPass			= _initProgram("src/renderer/OpenGL/shaders/shadingPass.vert", "src/renderer/OpenGL/shaders/shadingPass.frag");
				_uCamPosLoc				= glGetUniformLocation(_shadingPass, "uCamPos");
				_uLightPositionLoc		= glGetUniformLocation(_shadingPass, "uLightPosition");
				_uLightDirectionLoc		= glGetUniformLocation(_shadingPass, "uLightDirection");
				_uLightEmissivityLoc	= glGetUniformLocation(_shadingPass, "uLightEmissivity");

				_finalPass				= _initProgram("src/renderer/OpenGL/shaders/finalPass.vert", "src/renderer/OpenGL/shaders/finalPass.frag");
				_uGammaLoc				= glGetUniformLocation(_finalPass, "uGamma");

				glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.a);
				glGenVertexArrays(1, &_vaoEmpty);

				glGenFramebuffers(1, &_fboBasePass);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboBasePass);
				_generateAndAttachMap(&_resultMap, 0);
				_generateAndAttachMap(&_albedoMap, 1);
				_generateAndAttachMap(&_specularMap, 2);
				_generateAndAttachMap(&_shininessMap, 3);
				_generateAndAttachMap(&_normalMap, 4);
				_generateAndAttachMap(&_positionMap, 5);
				GLenum DrawBuffers0[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
				glDrawBuffers(6, DrawBuffers0);

				glGenRenderbuffers(1, &_rboBasePass);
				glBindRenderbuffer(GL_RENDERBUFFER, _rboBasePass);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rboBasePass);

				glGenFramebuffers(1, &_fboShadingPass);
				glBindFramebuffer(GL_FRAMEBUFFER, _fboShadingPass);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _resultMap, 0);
				GLenum DrawBuffers1[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, DrawBuffers1);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			void setVSync(bool p_vSync) override { _vSync = p_vSync; SDL_GL_SetSwapInterval(p_vSync); }
			void resize(const int p_width, const int p_height)  override { glViewport(0, 0, p_width, p_height); };
			void drawFrame(SDL_Window* p_window) override;
			
			void createMesh(Scene::MeshTriangle* p_mesh) override { _meshes.insert(std::pair<Scene::MeshTriangle*, MeshOGL*>(p_mesh, new MeshOGL(p_mesh))); }
			void deleteMesh(Scene::MeshTriangle* p_mesh) override { delete _meshes.at(p_mesh); _meshes.erase(_meshes.find(p_mesh)); }

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _basePass			= GL_INVALID_INDEX;
			GLint  _uMatrix_MVPLoc		= GL_INVALID_INDEX;
			GLint  _uMatrix_MLoc		= GL_INVALID_INDEX;
			GLint  _uMatrix_NormalLoc	= GL_INVALID_INDEX;
			GLint  _uAmbientLoc			= GL_INVALID_INDEX;
			GLint  _uDiffuseLoc			= GL_INVALID_INDEX;
			GLint  _uSpecularLoc		= GL_INVALID_INDEX;
			GLint  _uShininessLoc		= GL_INVALID_INDEX;
			GLint  _uHasAmbientMapLoc	= GL_INVALID_INDEX;
			GLint  _uHasDiffuseMapLoc	= GL_INVALID_INDEX;
			GLint  _uHasSpecularMapLoc	= GL_INVALID_INDEX;
			GLint  _uHasShininessMapLoc = GL_INVALID_INDEX;
			GLint  _uHasNormalMapLoc	= GL_INVALID_INDEX;

			GLuint _shadingPass			= GL_INVALID_INDEX;
			GLuint _uCamPosLoc			= GL_INVALID_INDEX;
			GLuint _uLightPositionLoc	= GL_INVALID_INDEX;
			GLuint _uLightDirectionLoc	= GL_INVALID_INDEX;
			GLuint _uLightEmissivityLoc = GL_INVALID_INDEX;

			GLuint _finalPass			= GL_INVALID_INDEX;
			GLint  _uGammaLoc			= GL_INVALID_INDEX;

			GLuint _fboBasePass			= GL_INVALID_INDEX;
			GLuint _rboBasePass			= GL_INVALID_INDEX;
			GLuint _resultMap			= GL_INVALID_INDEX;
			GLuint _albedoMap			= GL_INVALID_INDEX;
			GLuint _specularMap			= GL_INVALID_INDEX;
			GLuint _shininessMap		= GL_INVALID_INDEX;
			GLuint _normalMap			= GL_INVALID_INDEX;
			GLuint _positionMap			= GL_INVALID_INDEX;

			GLuint _fboShadingPass		= GL_INVALID_INDEX;
			GLuint _vaoEmpty			= GL_INVALID_INDEX;

			std::map<Scene::MeshTriangle*,MeshOGL*> _meshes;

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void _collectTexture();
			void _computeShading();
			void _applyPostProcess();

			void _generateAndAttachMap(GLuint* p_texture,int p_id);
			
			std::string _readShader(std::string p_path);
			void _readCompileShader(GLuint p_shader, std::string p_path);
			GLuint _initProgram(std::string p_pathVert, std::string p_pathFrag);
		};
	}
}
#endif