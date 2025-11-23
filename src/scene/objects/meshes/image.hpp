#pragma once

#include "utils/defines.hpp"
#include <vector>

namespace M3D
{
	class Image
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Image(uint p_width, uint p_height, uint p_bitDepth, uint p_nbChannels, uchar* p_data) : _width(p_width), _height(p_height), _bitDepth(p_bitDepth), _nbChannels(p_nbChannels), _data(p_data, &p_data[p_width * p_height * (p_bitDepth / 8) * p_nbChannels]) {}
		~Image() {}

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline uint getHeight() const { return _width; }
		inline uint getWidth() const { return _height; }
		inline uint getBitDepth() const { return _bitDepth; }
		inline uint getNbChannels() const { return _nbChannels; }
		inline const uchar* getData() const { return _data.data(); }

	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		uint _width;
		uint _height;
		uint _bitDepth;
		uint _nbChannels;
		std::vector<uchar> _data;
	};
}
