#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

namespace M3D
{
	class Image {
	public:
		// --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ----------------------------------------------
		Image(int p_width, int p_height, int p_nbChannels, int p_bitDepth, int p_format, unsigned char* p_data) :
			_width(p_width), _height(p_height), _nbChannels(p_nbChannels), _bitDepth(p_bitDepth), _format(p_format), _data(p_data) {}
		~Image(){}

		// ----------------------------------------------------- GETTERS -------------------------------------------------------
		inline int getHeight() const { return _width; }
		inline int getWidth() const { return _height; }
		inline int getNbChannels() const { return _nbChannels; }
		inline int getBitDepth() const { return _bitDepth; }
		inline int getFormat() const { return _format; }
		inline unsigned char& getData() const { return *_data; }
		
	private:
		// ----------------------------------------------------- ATTRIBUTS -----------------------------------------------------
		int _width;
		int _height;
		int _nbChannels;
		int _bitDepth;
		int _format;
		unsigned char* _data;
	};
}

#endif