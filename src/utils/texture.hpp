#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "image.hpp"

namespace M3D
{

	enum MAGNIFICATION_TYPE { MAG_LINEAR, MAG_NEAREST };
	enum MINIFICATION_TYPE { MIN_LINEAR, MIN_NEAREST, MIN_NEAREST_MIPMAP_NEAREST, MIN_NEAREST_MIPMAP_LINEAR, MIN_LINEAR_MIPMAP_NEAREST, MIN_LINEAR_MIPMAP_LINEAR };
	enum WRAPPING_TYPE { WRAP_REPEAT, WRAP_MIRRORED_REPEAT, WRAP_CLAMP_TO_EDGE };

	struct Texture {
		MINIFICATION_TYPE	_minification	= MINIFICATION_TYPE::MIN_NEAREST;
		MAGNIFICATION_TYPE	_magnification	= MAGNIFICATION_TYPE::MAG_NEAREST;
		WRAPPING_TYPE		_wrappingR		= WRAPPING_TYPE::WRAP_REPEAT;
		WRAPPING_TYPE		_wrappingS		= WRAPPING_TYPE::WRAP_REPEAT;
		WRAPPING_TYPE		_wrappingT		= WRAPPING_TYPE::WRAP_REPEAT;
		Image*				_image			= nullptr;
	};
}

#endif