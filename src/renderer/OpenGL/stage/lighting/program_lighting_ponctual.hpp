#ifndef __PROGRAM_LIGHTING_PONCTUAL_OGL_HPP__
#define __PROGRAM_LIGHTING_PONCTUAL_OGL_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "../program_ogl.hpp"

#include "application.hpp"
#include "utils/define.hpp"
#include "scene/objects/lights/light.hpp"
#include "scene/scene_graph/scene_graph_node.hpp"

namespace M3D
{
	namespace Renderer
	{
		class ProgramLightingPonctualOGL : public ProgramOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ProgramLightingPonctualOGL() : ProgramOGL("src/renderer/OpenGL/shaders/utils/billboard.vert", "src/renderer/OpenGL/shaders/shading/ponctualLightingPass.frag") {
				_uCamDataLoc			= glGetUniformLocation(_program, "uCamData");
				_uLightPositionLoc		= glGetUniformLocation(_program, "uLightPosition");
				_uLightDirectionLoc		= glGetUniformLocation(_program, "uLightDirection");
				_uLightEmissivityLoc	= glGetUniformLocation(_program, "uLightEmissivity");
				_uLightCosAnglesLoc		= glGetUniformLocation(_program, "uLightCosAngles");

				glCreateBuffers(1, &_vbo);
				glCreateBuffers(1, &_ebo);
				glCreateVertexArrays(1, &_vao);

				glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(Vec3f));
				glEnableVertexArrayAttrib(_vao, 0);
				glVertexArrayAttribFormat(_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_vao, 0, 0);

				unsigned int indices[] = {0,1,2,2,3,0};
				glNamedBufferData(_ebo, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
				glNamedBufferData(_vbo, 4 * sizeof(Vec3f)		, nullptr, GL_DYNAMIC_DRAW);

				glVertexArrayElementBuffer(_vao, _ebo);
			}

			~ProgramLightingPonctualOGL() {
				glDisableVertexArrayAttrib(_vao, 0);
				glDeleteBuffers(1, &_vbo);
				glDeleteBuffers(1, &_ebo);
				glDeleteVertexArrays(1, &_vao);
			}

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override {}

			void execute(Scene::SceneGraphNode* p_instanceLight, Scene::Light* p_light, float p_zfar,
						 GLuint p_positionMap, GLuint p_normalMetalnessMap, GLuint p_albedoRoughnessMap, GLuint p_shadowMap, GLuint p_fboShading) {
				glBindFramebuffer(GL_FRAMEBUFFER, p_fboShading);

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);

				glUseProgram(_program);

				glBindTextureUnit(0, p_positionMap);
				glBindTextureUnit(1, p_normalMetalnessMap);
				glBindTextureUnit(2, p_albedoRoughnessMap);
				glBindTextureUnit(3, p_shadowMap);

				Mat4f p_matrixVP = Application::getInstance().getSceneManager().getMainCameraViewMatrix() * Application::getInstance().getSceneManager().getMainCameraProjectionMatrix();
				Vec4f minBillboard = p_matrixVP * Vec4f(p_instanceLight->getLeft()*p_light->getRange()  + p_instanceLight->getDown()*p_light->getRange(),1.);
				minBillboard /= minBillboard.a;
				Vec4f maxBillboard = p_matrixVP * Vec4f(p_instanceLight->getRight()*p_light->getRange() + p_instanceLight->getUp()*p_light->getRange(),1.);
				maxBillboard /= maxBillboard.a;

				_billBoardCoord[0] = glm::clamp(Vec3f(minBillboard.x, minBillboard.y,  minBillboard.z)						,-VEC3F_ONE,VEC3F_ONE);
				_billBoardCoord[1] = glm::clamp(Vec3f(maxBillboard.x, minBillboard.y, (minBillboard.z+maxBillboard.z)*0.5)	,-VEC3F_ONE,VEC3F_ONE);
				_billBoardCoord[2] = glm::clamp(Vec3f(maxBillboard.x, maxBillboard.y,  maxBillboard.z)						,-VEC3F_ONE,VEC3F_ONE);
				_billBoardCoord[3] = glm::clamp(Vec3f(minBillboard.x, maxBillboard.y, (minBillboard.z+maxBillboard.z)*0.5)	,-VEC3F_ONE,VEC3F_ONE);

				glProgramUniform4fv(_program, _uCamDataLoc,			1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition(), p_zfar)));
				glProgramUniform3fv(_program, _uLightPositionLoc,	1, glm::value_ptr(p_instanceLight->getPosition()));
				glProgramUniform3fv(_program, _uLightDirectionLoc,	1, glm::value_ptr(p_instanceLight->getFront()));
				glProgramUniform3fv(_program, _uLightEmissivityLoc, 1, glm::value_ptr(p_light->getEmissivity()));
				glProgramUniform2fv(_program, _uLightCosAnglesLoc,	1, glm::value_ptr(Vec2f(p_light->getCosInnerConeAngle(),p_light->getCosOuterConeAngle())));
				glNamedBufferSubData(_vbo, 0, 4 * sizeof(Vec3f), _billBoardCoord);

				glBindVertexArray(_vao);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				glDisable(GL_BLEND);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
			GLuint _vao					= GL_INVALID_INDEX;
			GLuint _vbo					= GL_INVALID_INDEX;
			GLuint _ebo					= GL_INVALID_INDEX;
			Vec3f _billBoardCoord[4];

			GLuint _uCamDataLoc			= GL_INVALID_INDEX;
			GLuint _uLightPositionLoc	= GL_INVALID_INDEX;
			GLuint _uLightDirectionLoc	= GL_INVALID_INDEX;
			GLuint _uLightEmissivityLoc = GL_INVALID_INDEX;
			GLuint _uLightCosAnglesLoc	= GL_INVALID_INDEX;
		};
	}
}
#endif