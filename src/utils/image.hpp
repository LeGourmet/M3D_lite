#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include "stb/stb_image.h"

#include <string>
#include <iostream>

namespace M3D
{
	class Image
	{
	public:
		Image();
		~Image();

		bool load(const std::string p_path);
		// save
	public:
		int			   _width = 0;
		int			   _height = 0;
		int			   _nbChannels = 0;
		unsigned char* _pixels = nullptr;
	};
}

#endif