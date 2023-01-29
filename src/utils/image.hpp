#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <string>

namespace M3D
{
namespace Image
{
	unsigned char* load(std::string p_path, int* p_width, int* p_height, int* p_nbChannels);
	void free(unsigned char* p_image);
}
}

#endif