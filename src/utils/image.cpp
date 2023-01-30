#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace M3D
{
namespace Image
{

	unsigned char* load(std::string p_path, int* p_width, int* p_height, int* p_nbChannels) {
		stbi_set_flip_vertically_on_load(true); 
		return stbi_load(p_path.c_str(), p_width, p_height, p_nbChannels, 0);
	}

	void free(unsigned char* p_image) { stbi_image_free(p_image); }

}
}