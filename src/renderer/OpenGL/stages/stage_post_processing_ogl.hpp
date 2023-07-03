#ifndef __STAGE_POST_PROCESSING_OGL_HPP__
#define __STAGE_POST_PROCESSING_OGL_HPP__

#include "GL/gl3w.h"

#include "stage_ogl.hpp"

#include "application.hpp"
#include "renderer/renderer.hpp"
#include "scene/objects/meshes/mesh.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"
#include "renderer/OpenGL/texture_ogl.hpp"
#include "renderer/OpenGL/program_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class StagePostProcessingOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StagePostProcessingOGL() { 
				// --- bloom ---
				glCreateFramebuffers(1, &_fboBloom);
				generateMap(&_bloomMap, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				// generate mipmap ??
				attachColorMap(_fboBloom, _bloomMap, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0};
				glNamedFramebufferDrawBuffers(_fboBloom, 1, DrawBuffers);

				// --- tone mapping ---
				_toneMappingPass.addUniform("uGamma");

				glCreateVertexArrays(1, &_emptyVAO);
			}

			~StagePostProcessingOGL() { 
				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ------------------------------------------------------ GETTERS ------------------------------------------------------
			GLuint getShadingMap() { return _shadingMap; }

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) { }

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_HDRMap) {
				glViewport(0, 0, p_width, p_height);

				// use bloom down *8 ou +- with p_hdr
				// use bloom up   *7 ou +- with down

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_toneMappingPass.getProgram());

				// todo bind bloom
				glProgramUniform1f(_toneMappingPass.getProgram(), _toneMappingPass.getUniform("uGamma"), Application::getInstance().getRenderer().getGamma());
				glBindTextureUnit(0, p_HDRMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fboBloom	= GL_INVALID_INDEX;
			
			GLuint _shadingMap	= GL_INVALID_INDEX;
			GLuint _bloomMap	= GL_INVALID_INDEX;
			
			GLuint _emptyVAO	= GL_INVALID_INDEX;

			ProgramOGL _toneMappingPass = ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/toneMappingPass.frag");
			ProgramOGL _bloomDownSamplePass = ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/bloomDownSamplePass.frag");
			ProgramOGL _bloomUpSamplePass = ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/bloomUpSamplePass.frag");
		};
	}
}

#endif
