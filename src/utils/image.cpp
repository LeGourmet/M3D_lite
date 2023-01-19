#include "image.hpp"

namespace M3D
{
	Image::Image() {}
	Image::~Image() { /*stbi_image_free(_pixels);*/ }

	bool Image::load(const std::string p_path) {
		//_pixels = stbi_load(p_path.c_str(), &_width, &_height, &_nbChannels, 0);
		_pixels = nullptr;
		return _pixels != nullptr;
	}
}