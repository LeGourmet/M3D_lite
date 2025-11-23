#pragma once

#include "GL/gl3w.h"

#include "stage_ogl.hpp"

#include "application.hpp"
#include "renderer/renderer.hpp"
#include "scene/objects/meshes/mesh.hpp"
#include "renderer/ogl_mesh.hpp"
#include "renderer/ogl_texture.hpp"
#include "renderer/ogl_program.hpp"

#include "lodepng.h"

namespace M3D
{
	class StagePostProcessingOGL : public StageOGL {
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		StagePostProcessingOGL() {
			// --- anti-aliasing ---
			_FXAAPass.addUniform("uInvSrcRes");
			_SMAAPass.addUniform("uInvSrcRes");

			glCreateFramebuffers(1, &_fboAA);
			generateMap(&_aaMap, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
			attachColorMap(_fboAA, _aaMap, 0);
			GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glNamedFramebufferDrawBuffers(_fboAA, 1, DrawBuffers);

			// --- bloom ---
			_BloomDownSamplePass.addUniform("uInvSrcRes");
			_BloomUpSamplePass.addUniform("uInvSrcRes");

			glCreateFramebuffers(1, &_fboBloom);

			// --- tone mapping and final mix ---
			_FinalPass.addUniform("uBloomPower");

			glCreateTextures(GL_TEXTURE_2D, 1, &_AgXLUT);
			glTextureParameteri(_AgXLUT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_AgXLUT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(_AgXLUT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_AgXLUT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			uint width = 0u, height = 0u;
			std::vector<uchar> fileData;
			if (lodepng::load_file(fileData, "resources/images/AgX_lut.png"))	throw std::runtime_error("Image invalid !");

			std::vector<uchar> image;
			if (lodepng::decode(image, width, height, fileData.data(), fileData.size(), LodePNGColorType::LCT_RGB, 48)) throw std::runtime_error("Image invalid !");

			glBindTexture(GL_TEXTURE_2D, _AgXLUT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, image.data());

			glCreateVertexArrays(1, &_emptyVAO);
		}

		~StagePostProcessingOGL() {
			glDeleteTextures(1, &_aaMap);
			glDeleteFramebuffers(1, &_fboAA);

			glDeleteFramebuffers(1, &_fboBloom);
			for (uint i = 1; i < _bloomMaps.size(); i++) glDeleteTextures(1, &_bloomMaps[i]);

			glDeleteTextures(1, &_AgXLUT);

			glDeleteVertexArrays(1, &_emptyVAO);
		}

		// ----------------------------------------------------- FONCTIONS -----------------------------------------------------
		void resize(uint p_width, uint p_height) {
			resizeColorMap(GL_RGB16F, GL_RGB, GL_FLOAT, p_width, p_height, _aaMap);

			for (uint i = 1; i < _bloomMaps.size(); i++) glDeleteTextures(1, &_bloomMaps[i]);

			uint lvMipMap = glm::max<uint>(0, (uint)glm::floor(glm::log2(glm::min<uint>(p_width, p_height)) - 3));

			_bloomMaps.resize(lvMipMap * 2 + 1);
			_bloomMapsDims.resize(lvMipMap * 2 + 1);
			_bloomMapsDims[0] = Vec2f(p_width, p_height);

			for (uint i = 1; i < lvMipMap + 1;i++) {
				generateMap(&_bloomMaps[i], GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				_bloomMapsDims[i] = Vec2f(_bloomMapsDims[i - 1].x, _bloomMapsDims[i - 1].y) * 0.5f;
				resizeColorMap(GL_RGB16F, GL_RGB, GL_FLOAT, (uint)_bloomMapsDims[i].x, (uint)_bloomMapsDims[i].y, _bloomMaps[i]);
			}

			for (uint i = lvMipMap + 1; i < lvMipMap * 2 + 1;i++) {
				generateMap(&_bloomMaps[i], GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				_bloomMapsDims[i] = Vec2f(_bloomMapsDims[i - 1].x, _bloomMapsDims[i - 1].y) * 2.f;
				resizeColorMap(GL_RGB16F, GL_RGB, GL_FLOAT, (uint)_bloomMapsDims[i].x, (uint)_bloomMapsDims[i].y, _bloomMaps[i]);
			}
		}

		void execute(uint p_width, uint p_height, std::map<Mesh*, MeshOGL*> p_meshes, std::map<Texture*, TextureOGL*> p_textures, GLuint p_HDRMap) {
			// --- anti-aliasing ---
			glViewport(0, 0, p_width, p_height);
			glBindFramebuffer(GL_FRAMEBUFFER, _fboAA);
			glClear(GL_COLOR_BUFFER_BIT);
			//if (p_AAType == AA_TYPE::NONE) {
			glCopyImageSubData(p_HDRMap, GL_TEXTURE_2D, 0, 0, 0, 0, _aaMap, GL_TEXTURE_2D, 0, 0, 0, 0, p_width, p_height, 1);
			/* }
			else if (p_AAType == AA_TYPE::FXAA) {
				glUseProgram(_FXAAPass.getProgram());

				glProgramUniform2fv(_FXAAPass.getProgram(), _FXAAPass.getUniform("uInvSrcRes"), 1, glm::value_ptr(1.f / Vec2f(p_width, p_height)));
				glBindTextureUnit(0, p_HDRMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);
			}
			else if (p_AAType == AA_TYPE::SMAA) {
				glUseProgram(_SMAAPass.getProgram());

				glProgramUniform2fv(_SMAAPass.getProgram(), _SMAAPass.getUniform("uInvSrcRes"), 1, glm::value_ptr(1.f / Vec2f(p_width, p_height)));
				glBindTextureUnit(0, p_HDRMap);

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);
			}*/
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// --- bloom ---
			glBindFramebuffer(GL_FRAMEBUFFER, _fboBloom);

			_bloomMaps[0] = _aaMap;

			// --- Down sample ---
			glUseProgram(_BloomDownSamplePass.getProgram());

			for (uint i = 1; i < uint((_bloomMaps.size() - 1) * 0.5f) + 1;i++) {
				glViewport(0, 0, (uint)_bloomMapsDims[i].x, (uint)_bloomMapsDims[i].y);
				attachColorMap(_fboBloom, _bloomMaps[i], 0);

				glBindTextureUnit(0, _bloomMaps[i - 1]);
				glProgramUniform2fv(_BloomDownSamplePass.getProgram(), _BloomDownSamplePass.getUniform("uInvSrcRes"), 1, glm::value_ptr(1.f / _bloomMapsDims[i - 1]));

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);
			}

			// --- Up sample ---
			glUseProgram(_BloomUpSamplePass.getProgram());

			for (uint i = uint((_bloomMaps.size() - 1) * 0.5f) + 1; i < _bloomMaps.size();i++) {
				glViewport(0, 0, (uint)_bloomMapsDims[i].x, (uint)_bloomMapsDims[i].y);
				attachColorMap(_fboBloom, _bloomMaps[i], 0);

				glBindTextureUnit(0, _bloomMaps[i - 1]);
				glBindTextureUnit(1, _bloomMaps[i - 2]);
				glProgramUniform2fv(_BloomUpSamplePass.getProgram(), _BloomUpSamplePass.getUniform("uInvSrcRes"), 1, glm::value_ptr(1.f / _bloomMapsDims[i - 1]));

				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// --- Tone Mapping ---
			glViewport(0, 0, p_width, p_height);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(_FinalPass.getProgram());

			//glProgramUniform1f(_FinalPass.getProgram(), _FinalPass.getUniform("uBloomPower"), p_bloomPower);
			glProgramUniform1f(_FinalPass.getProgram(), _FinalPass.getUniform("uBloomPower"), 0.04f);
			glBindTextureUnit(0, _aaMap);
			glBindTextureUnit(1, _bloomMaps[_bloomMaps.size() - 1]);
			glBindTextureUnit(2, _AgXLUT);

			glBindVertexArray(_emptyVAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		GLuint _fboBloom = GL_INVALID_INDEX;
		GLuint _fboAA = GL_INVALID_INDEX;

		GLuint _aaMap = GL_INVALID_INDEX;

		GLuint _AgXLUT = GL_INVALID_INDEX;

		std::vector<GLuint> _bloomMaps;
		std::vector<Vec2f>	_bloomMapsDims;

		GLuint _emptyVAO = GL_INVALID_INDEX;

		ProgramOGL _FXAAPass = ProgramOGL("resources/shaders/utils/QuadScreen.vert", "", "resources/shaders/post_processing/FXAA.frag");
		ProgramOGL _SMAAPass = ProgramOGL("resources/shaders/utils/QuadScreen.vert", "", "resources/shaders/post_processing/SMAA.frag");
		ProgramOGL _BloomDownSamplePass = ProgramOGL("resources/shaders/utils/QuadScreen.vert", "", "resources/shaders/post_processing/BloomDownSample.frag");
		ProgramOGL _BloomUpSamplePass = ProgramOGL("resources/shaders/utils/QuadScreen.vert", "", "resources/shaders/post_processing/BloomUpSample.frag");
		ProgramOGL _FinalPass = ProgramOGL("resources/shaders/utils/QuadScreen.vert", "", "resources/shaders/post_processing/FinalPass.frag");
	};
}
