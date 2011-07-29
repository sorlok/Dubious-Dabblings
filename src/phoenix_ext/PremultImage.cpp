#include "PremultImage.hpp"

#include <iostream>

using namespace phoenix;
using std::initializer_list;
using std::vector;


PremultImage::PremultImage() : buffer_(NULL), size_({0,0,0,0})
{

}

PremultImage::~PremultImage()
{
	if (buffer_) {
		delete [] buffer_;
		buffer_ = NULL;
	}
}


void PremultImage::resetSize(const Geometry& size, std::vector<uint8_t> fill)
{
	//Delete the old one
	if (buffer_) {
		delete [] buffer_;
		buffer_ = NULL;
	}

	//Silently return if the new size is zero
	size_ = size;
	size_t dim = size.width*size.height;
	if (dim==0) {
		return;
	}

	//Get the background color:
	uint32_t color = PremultImage::DecodeColor(fill);

	std::cout <<"Resize: " <<size.width <<"," <<size.height <<"\n";

	//Allocate the new one
	buffer_ = new uint32_t[dim];

	//As fun as it might be to allocate the image with garbage
	//  pixel data, I suppose we should zero it (grumble grumble)
	std::fill_n(buffer_, dim, color);
}

const phoenix::Geometry& PremultImage::getSize() const
{
	return size_;
}


uint32_t& PremultImage::operator[] (size_t x)
{
	return buffer_[x];
}

uint32_t* PremultImage::getPixels()
{
	return buffer_;
}


uint32_t PremultImage::Premultiply(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	/*r = (a * r + 127) / 255;
    g = (a * g + 127) / 255;
    b = (a * b + 127) / 255;*/
	///ugh... not working right.

    return (a << 24) | (r << 16) | (g << 8) | b;
}

uint32_t PremultImage::Premultiply(uint32_t argb)
{
	return PremultImage::Premultiply(((argb>>24)&0xFF), ((argb>>16)&0xFF), ((argb>>8)&0xFF), (argb&0xFF));
}

void PremultImage::Combine(uint32_t& base, uint32_t brush)
{
	//TODO: We will eventually use a "color" class, so that we don't have to separate out components...
	/*uint32_t r0 = ((base>>16)&0xFF);
	uint32_t g0 = ((base>>8)&0xFF);
	uint32_t b0 = (base&0xFF);
	uint32_t r1 = ((brush>>16)&0xFF);
	uint32_t g1 = ((brush>>8)&0xFF);
	uint32_t b1 = (brush&0xFF);*/

	//NOTE: Seems like we should just be able to hack it with pointers.
	//uint8_t* basePtr = reinterpret_cast<uint8_t*>(&base);
	//NOTE: This is a bad idea.
	uint8_t a0 = ((base>>24)&0xFF);
	uint8_t r0 = ((base>>16)&0xFF);
	uint8_t g0 = ((base>>8)&0xFF);
	uint8_t b0 = (base&0xFF);
	uint8_t a1 = ((brush>>24)&0xFF);
	uint8_t r1 = ((brush>>16)&0xFF);
	uint8_t g1 = ((brush>>8)&0xFF);
	uint8_t b1 = (brush&0xFF);

	/*uint8_t a = ((a0*a0)>>8) + ((a1*(0xFF-a0))>>8);
	uint8_t r = ((r0*a0)>>8) + ((r1 * (0xFF - a1))>>8);
	uint8_t g = ((g0*a0)>>8) + ((g1 * (0xFF - a1))>>8);
	uint8_t b = ((b0*a0)>>8) + ((b1 * (0xFF - a1))>>8);*/

	uint8_t deltR = r1 - r0;
	uint8_t deltG = g1 - g0;
	uint8_t deltB = b1 - b0;
	r0 += (deltR*a1)/255;
	g0 += (deltG*a1)/255;
	b0 += (deltB*a1)/255;

	a0 = a1 + a0;

	base = (a0<<24) | (r0<<16) | (g0<<8) | (b0);
}

uint32_t PremultImage::DecodeColor(const vector<uint8_t>& color)
{
	auto it = color.begin();
	if (color.size()==1) {
		return Premultiply(0xFF, color[0], color[0], color[0]);
	} else if (color.size()==2) {
		return Premultiply(color[0], color[1], color[1], color[1]);
	} else if (color.size()==3) {
		return Premultiply(0xFF, color[0], color[1], color[2]);
	} else if (color.size()==4) {
		return Premultiply(color[0], color[1], color[2], color[3]);
	} else {
		throw std::logic_error("Color requires 1, 2, 3, or 4 components");
	}
}


void PremultImage::fillRect(const Geometry& rectangle, vector<uint8_t> color)
{
	std::cout <<"Filling rectangle [" <<rectangle.x <<"," <<rectangle.y <<"," <<rectangle.width <<"," <<rectangle.height <<"]\n";

	//Get the color
	uint32_t color_ = PremultImage::DecodeColor(color);

	//Fill the rectangle; ensure we never stray outside the buffer's boundaries.
	//TODO: We should make judicious use of multiple memcpy calls...
	//TODO: We need to change out "Geometry" for a "Rectangle" which takes
	//      coordinates of type int, since negative values are totally ok.
	for (int destY=rectangle.y; destY<rectangle.y+rectangle.height; destY++) {
		for (int destX=rectangle.x; destX<rectangle.x+rectangle.width; destX++) {
			if (destX>=0 && destY>=0 && destX<=size_.width && destY<=size_.height) {
				Combine(buffer_[destY*size_.width + destX], color_);
			}
		}
	}
}




