#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include "stb/stb_image.h"

#include <string>
#include <iostream>

namespace M3D
{
	struct Image
	{
		Image() {}
		~Image() { stbi_image_free(_pixels); }

		bool load(const std::string p_path) {
			_pixels = stbi_load(p_path.c_str(), &_width, &_height, &_nbChannels, 0);
			return _pixels != nullptr;
		}

		int			   _width = 0;
		int			   _height = 0;
		int			   _nbChannels = 0;
		unsigned char* _pixels = nullptr;
	};
}

#endif