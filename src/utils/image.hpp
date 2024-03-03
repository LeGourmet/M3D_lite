#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <vector>

namespace M3D
{
	class Image 
	{
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Image(int p_width, int p_height, int p_bitDepth, int p_nbChannels, unsigned char* p_data) : _width(p_width), _height(p_height), _bitDepth(p_bitDepth), _nbChannels(p_nbChannels), _data(p_data, &p_data[p_width * p_height * (p_bitDepth / 8) * p_nbChannels]) {}
		~Image(){}

		// ------------------------------------------------------ GETTERS ------------------------------------------------------
		inline int getHeight() const { return _width; }
		inline int getWidth() const { return _height; }
		inline int getBitDepth() const { return _bitDepth; }
		inline int getNbChannels() const { return _nbChannels; }
		inline const unsigned char* getData() const { return _data.data(); }
		
	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		int _width;
		int _height;
		int _bitDepth;
		int _nbChannels;
		std::vector<unsigned char> _data;
	};
}

#endif
