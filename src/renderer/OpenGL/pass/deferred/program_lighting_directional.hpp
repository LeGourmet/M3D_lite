#ifndef __PROGRAM_LIGHTING_DIRECTIONAL_OGL_HPP__
#define __PROGRAM_LIGHTING_DIRECTIONAL_OGL_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "../program_ogl.hpp"

#include "scene/objects/lights/light.hpp"
#include "scene/objects/meshes/mesh.hpp"
#include "scene/scene_graph/scene_graph_node.hpp"
#include "renderer/OpenGL/mesh_ogl.hpp"

namespace M3D
{
	namespace Renderer
	{
		class ProgramLightingDirectionalOGL : public ProgramOGL {
		public:
			// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
			ProgramLightingDirectionalOGL() : ProgramOGL("src/renderer/OpenGL/shaders/utils/quadScreen.vert", "src/renderer/OpenGL/shaders/shading/directionalLightingPass.frag") {
				_uCamDataLoc			= glGetUniformLocation(_program, "uCamDataPos");
				_uLightPositionLoc		= glGetUniformLocation(_program, "uLightPosition");
				_uLightDirectionLoc		= glGetUniformLocation(_program, "uLightDirection");
				_uLightEmissivityLoc	= glGetUniformLocation(_program, "uLightEmissivity");

				glCreateVertexArrays(1, &_emptyVAO);
			}

			~ProgramLightingDirectionalOGL() {
				glDeleteVertexArrays(1, &_emptyVAO);
			}

			// ---------------------------------------------------- FONCTIONS ------------------------------------------------------
			void resize(int p_width, int p_height) override {}

			void execute(Scene::SceneGraphNode* p_instanceLight, Scene::Light* p_light, const Vec3f& p_lightPos, float p_zfar,
						 GLuint p_positionMap, GLuint p_normalMetalnessMap, GLuint p_albedoRoughnessMap, GLuint p_shadowMap, GLuint p_fboShading) {
				glBindFramebuffer(GL_FRAMEBUFFER, p_fboShading);

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);

				glUseProgram(_program);

				glBindTextureUnit(0, p_positionMap);
				glBindTextureUnit(1, p_normalMetalnessMap);
				glBindTextureUnit(2, p_albedoRoughnessMap);
				glBindTextureUnit(3, p_shadowMap);

				glProgramUniform4fv(_program, _uCamDataLoc,			1, glm::value_ptr(Vec4f(Application::getInstance().getSceneManager().getMainCameraSceneGraphNode()->getPosition(), p_zfar)));
				glProgramUniform3fv(_program, _uLightPositionLoc,	1, glm::value_ptr(p_lightPos));
				glProgramUniform3fv(_program, _uLightDirectionLoc,	1, glm::value_ptr(p_instanceLight->getFront()));
				glProgramUniform3fv(_program, _uLightEmissivityLoc, 1, glm::value_ptr(p_light->getEmissivity()));
				
				glBindVertexArray(_emptyVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glBindVertexArray(0);

				glDisable(GL_BLEND);
			}

		private:
			// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------		
			GLuint _emptyVAO			= GL_INVALID_INDEX;

			GLuint _uCamDataLoc			= GL_INVALID_INDEX;
			GLuint _uLightPositionLoc	= GL_INVALID_INDEX;
			GLuint _uLightDirectionLoc	= GL_INVALID_INDEX;
			GLuint _uLightEmissivityLoc = GL_INVALID_INDEX;
		};
	}
}
#endif