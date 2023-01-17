#include "renderer_ogl.hpp"

#include "utils/define.hpp"
#include "application.hpp"

#include "glm/gtc/type_ptr.hpp"

namespace M3D
{
	namespace Renderer
	{
		void RendererOGL::init(SDL_Window* p_window) {
			glEnable(GL_DEPTH_TEST);

			_initProgram(); // throw exeption ?
							// use multiple pass (add _programme inside vector)
			// get unifom var
			
			glUseProgram(_program);
		}

		void RendererOGL::drawFrame() {
			SceneManager scene = Application::getInstance().getSceneManager().getMeshes();
			Camera camera = Application::getInstance().getSceneManager().getCamera();

			glClearColor(0., 0., 0., 1.); // define.hpp ? => here because color can change
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glProgramUniform3fv(_program, _uCamPosLoc, glm::value_ptr(camera.getPosition()));

			for (MeshTriangle mesh : scene.getMeshes()) {
				glProgramUniformMatrix4fv(_program, _uMVPMatrixLoc, 1, false, glm::value_ptr(camera.getProjectionMatrix() * camera.getViewMatrix() * mesh._transformation));
				
				glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasAmbientMap"), mesh._hasAmbientMap);
				glProgramUniform3fv(_program, glGetUniformLocation(_program, "uAmbient"), 1, glm::value_ptr(mesh._ka));
				if (mesh._hasAmbientMap) { mesh.getAmbientMap().bind(0); }	// glBindTextureUnit(0, id);

				glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasDiffuseMap"), mesh._hasDiffuseMap);
				glProgramUniform3fv(_program, glGetUniformLocation(_program, "uDiffuse"), 1, glm::value_ptr(mesh._kd));
				if (mesh._hasDiffuseMap) { mesh.getAmbientMap().bind(1); }	//  glBindTextureUnit(1, id);

				glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasSpecularMap"), mesh._hasSpecularMap);
				glProgramUniform3fv(_program, glGetUniformLocation(_program, "uSpecular"), 1, glm::value_ptr(mesh._ks));
				if (mesh._hasSpecularMap) { mesh.getSpecularMap().bind(2); } //  glBindTextureUnit(2, id);

				glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasShininessMap"), mesh._hasShininessMap);
				glProgramUniform1f(_program, glGetUniformLocation(_program, "uShininess"), mesh._s);
				if (mesh._hasShininessMap) { mesh.getShininessMap().bind(3); } //  glBindTextureUnit(3, id);

				glProgramUniform1i(_program, glGetUniformLocation(_program, "uHasNormalMap"), mesh._hasNormalMap);
				if (mesh._hasNormalMap) { mesh.getNormalMap().bind(4); } // glBindTextureUnit(4, id);

				glBindVertexArray(_vao);
				glDrawElements(GL_TRIANGLES, mesh., GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

		}
	}
}