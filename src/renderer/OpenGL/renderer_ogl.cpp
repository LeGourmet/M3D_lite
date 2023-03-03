#include "renderer_ogl.hpp"

#include "application.hpp"
#include "scene/scene_manager.hpp"

#include <fstream>
#include <sstream>

namespace M3D
{
	namespace Renderer
	{
			void RendererOGL::drawFrame(SDL_Window* p_window){
				_geometryPass->execute(_viewport_width,_viewport_height,Application::getInstance().getSceneManager().getMeshes(),_meshes);
				_shadingPass->execute(_viewport_width,_viewport_height,_geometryPass->getPositionMetalnessMap(),_geometryPass->getNormalRoughnessMap(),_geometryPass->getAlbedoMap());
				_finalPass->execute(_viewport_width,_viewport_height,_gamma,_geometryPass->getAmbientMap());

				SDL_GL_SwapWindow(p_window);
			}

			void RendererOGL::resize(const int p_width, const int p_height) { 
				_viewport_width = p_width;
				_viewport_height = p_height; 

				_geometryPass->resize(p_width, p_height);
				_shadingPass->resize(p_width, p_height);
				_finalPass->resize(p_width, p_height);
			}
	}
}