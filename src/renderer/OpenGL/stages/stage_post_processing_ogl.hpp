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

#include <iostream>

namespace M3D
{
	namespace Renderer
	{
		class StagePostProcessingOGL : public StageOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			StagePostProcessingOGL() {
				// --- fxaa ---
				_fxaaPass.addUniform("uInvSrcRes");
				
				// --- bloom ---
				_bloomDownSamplePass.addUniform("uInvSrcRes");
				_bloomUpSamplePass.addUniform("uInvSrcRes");
				
				// --- tone mapping ---
				_toneMappingPass.addUniform("uGamma");
				_toneMappingPass.addUniform("uBloomPower");

				glCreateFramebuffers(1, &_fboAA);
				generateMap(&_aaMap, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
				attachColorMap(_fboAA, _aaMap, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0};
				glNamedFramebufferDrawBuffers(_fboAA, 1, DrawBuffers);

				glCreateVertexArrays(1, &_emptyVAO);
				glCreateFramebuffers(1, &_fboBloom);
			}

			~StagePostProcessingOGL() {
				glDeleteTextures(1, &_aaMap);
				glDeleteFramebuffers(1, &_fboAA);

				glDeleteFramebuffers(1, &_fboBloom);
				for(int i=1; i<_bloomMaps.size(); i++) glDeleteTextures(1, &_bloomMaps[i]);
				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
			void resize(int p_width, int p_height) {
				resizeColorMap(p_width, p_height, _aaMap);

				for(int i=1; i<_bloomMaps.size(); i++) glDeleteTextures(1, &_bloomMaps[i]);

				unsigned int lvMipMap = glm::max<int>(0,(unsigned int)glm::floor(glm::log2(glm::min<int>(p_width, p_height))-3));
				
				_bloomMaps.resize(lvMipMap*2+1);
				_bloomMapsDims.resize(lvMipMap*2+1);
				_bloomMapsDims[0] = Vec2f(p_width,p_height);

				for(unsigned int i=1; i<lvMipMap+1 ;i++){
					generateMap(&_bloomMaps[i], GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
					float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
					glTextureParameterfv(_bloomMaps[i], GL_TEXTURE_BORDER_COLOR, borderColor);
					_bloomMapsDims[i] = Vec2f(_bloomMapsDims[i-1].x, _bloomMapsDims[i-1].y)*0.5f;
					resizeColorMap((int)_bloomMapsDims[i].x, (int)_bloomMapsDims[i].y, _bloomMaps[i]);
				}
				for(unsigned int i=lvMipMap+1; i<lvMipMap*2+1 ;i++){
					generateMap(&_bloomMaps[i], GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE); 
					_bloomMapsDims[i] = _bloomMapsDims[i] = Vec2f(_bloomMapsDims[i-1].x, _bloomMapsDims[i-1].y)*2.f;
					resizeColorMap((int)_bloomMapsDims[i].x, (int)_bloomMapsDims[i].y, _bloomMaps[i]);
				}
			}

			void execute(int p_width, int p_height, std::map<Scene::Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_HDRMap) {
				// --- fxaa ---
				glViewport(0, 0, p_width, p_height);

				glBindFramebuffer(GL_FRAMEBUFFER, _fboAA);

				glClear(GL_COLOR_BUFFER_BIT);

				glUseProgram(_fxaaPass.getProgram());

				glProgramUniform2fv(_fxaaPass.getProgram(), _fxaaPass.getUniform("uInvSrcRes"), 1, glm::value_ptr(1.f/Vec2f(p_width,p_height)));
				glBindTextureUnit(0, p_HDRMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// --- bloom ---
				// todo use bloom for computing lens flare and lens ghosting
				glBindFramebuffer(GL_FRAMEBUFFER, _fboBloom);

				_bloomMaps[0] = _aaMap;

				// --- Down sample ---
				glUseProgram(_bloomDownSamplePass.getProgram());

				for(int i=1; i<int((_bloomMaps.size()-1)*0.5f)+1 ;i++) {
					glViewport(0, 0, (int)_bloomMapsDims[i].x, (int)_bloomMapsDims[i].y);
					attachColorMap(_fboBloom, _bloomMaps[i], 0);
					
					glBindTextureUnit(0, _bloomMaps[i-1]);
					glProgramUniform2fv(_bloomDownSamplePass.getProgram(), _bloomDownSamplePass.getUniform("uInvSrcRes"), 1, glm::value_ptr(1.f/_bloomMapsDims[i-1]));

					glBindVertexArray(_emptyVAO);
					glDrawArrays(GL_TRIANGLES, 0, 3);
					glBindVertexArray(0);
				}

				// --- Up sample ---
				glUseProgram(_bloomUpSamplePass.getProgram());

				for(int i=int((_bloomMaps.size()-1)*0.5f)+1; i<_bloomMaps.size() ;i++){
					glViewport(0, 0, (int)_bloomMapsDims[i].x, (int)_bloomMapsDims[i].y);
					attachColorMap(_fboBloom, _bloomMaps[i], 0);

					glBindTextureUnit(0, _bloomMaps[i-1]);
					glBindTextureUnit(1, _bloomMaps[i-2]);
					glProgramUniform2fv(_bloomUpSamplePass.getProgram(), _bloomUpSamplePass.getUniform("uInvSrcRes"), 1, glm::value_ptr(1.f/_bloomMapsDims[i-1]));
					
					glBindVertexArray(_emptyVAO);
					glDrawArrays(GL_TRIANGLES, 0, 3);
					glBindVertexArray(0);
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// --- Tone Mapping ---
				glViewport(0, 0, p_width, p_height);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(_toneMappingPass.getProgram());

				glProgramUniform1f(_toneMappingPass.getProgram(), _toneMappingPass.getUniform("uGamma"), Application::getInstance().getRenderer().getGamma());
				glProgramUniform1f(_toneMappingPass.getProgram(), _toneMappingPass.getUniform("uBloomPower"), Application::getInstance().getRenderer().getBloomPower());
				glBindTextureUnit(0, _aaMap);
				glBindTextureUnit(1, _bloomMaps[_bloomMaps.size()-1]);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _fboBloom	= GL_INVALID_INDEX;
			GLuint _fboAA		= GL_INVALID_INDEX;

			GLuint _aaMap		= GL_INVALID_INDEX;

			std::vector<GLuint> _bloomMaps;
			std::vector<Vec2f>	_bloomMapsDims;

			GLuint _emptyVAO	= GL_INVALID_INDEX;

			ProgramOGL _fxaaPass					= ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/fxaaPass.frag");
			ProgramOGL _bloomDownSamplePass			= ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/bloomDownSamplePass.frag");
			ProgramOGL _bloomUpSamplePass			= ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/bloomUpSamplePass.frag");
			ProgramOGL _toneMappingPass				= ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "", "src/renderer/OpenGL/shaders/post_processing/toneMappingPass.frag");
		};
	}
}

#endif
