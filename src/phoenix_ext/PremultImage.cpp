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


void PremultImage::resetSize(const Geometry& size)
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

	std::cout <<"Resize: " <<size.width <<"," <<size.height <<"\n";

	//Allocate the new one
	buffer_ = new uint32_t[dim];

	//As fun as it might be to allocate the image with garbage
	//  pixel data, I suppose we should zero it (grumble grumble)
	std::fill_n(buffer_, dim, 0x000000);
}

const phoenix::Geometry& PremultImage::getSize() const
{
	return size_;
}


uint32_t& PremultImage::operator[] (size_t x)
{
	return buffer_[x];
}


uint32_t PremultImage::Premultiply(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	r = (a * r + 127) / 255;
    g = (a * g + 127) / 255;
    b = (a * b + 127) / 255;

    return /*(a << 24) |*/ (r << 16) | (g << 8) | b;
}

void PremultImage::Combine(uint32_t& base, uint32_t brush)
{
	//TODO: We will eventually use a "color" class, so that we don't have to separate out components...
	uint32_t r0 = ((base>>16)&0xFF);
	uint32_t g0 = ((base>>8)&0xFF);
	uint32_t b0 = (base&0xFF);
	uint32_t r1 = ((brush>>16)&0xFF);
	uint32_t g1 = ((brush>>8)&0xFF);
	uint32_t b1 = (brush&0xFF);

	r0 += (0xFF-r0)*r1;
	g0 += (0xFF-g0)*g1;
	b0 += (0xFF-b0)*b1;

	base = (nall::min(r0, 0xFF) << 16) | (nall::min(g0, 0xFF) << 8) | nall::min(b0, 0xFF);
}


void PremultImage::fillRect(const Geometry& rectangle, vector<uint8_t> color)
{
	std::cout <<"Filling rectangle [" <<rectangle.x <<"," <<rectangle.y <<"," <<rectangle.width <<"," <<rectangle.height <<"]\n";

	//Get the color
	uint32_t color_;
	auto it = color.begin();
	if (color.size()==1) {
		color_ = Premultiply(0xFF, color[0], color[0], color[0]);
	} else if (color.size()==2) {
		color_ = Premultiply(color[0], color[1], color[1], color[1]);
	} else if (color.size()==3) {
		color_ = Premultiply(0xFF, color[0], color[1], color[2]);
	} else if (color.size()==4) {
		color_ = Premultiply(color[0], color[1], color[2], color[3]);
	} else {
		throw std::logic_error("Color requires 1, 2, 3, or 4 components");
	}

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




