#include "CanvasExt.hpp"

#include <iostream>

using namespace phoenix;


CanvasExt::CanvasExt() : Canvas(), image_(), offset_({0,0,0,0})
{
}



PremultImage& CanvasExt::getBufferedImage()
{
	return image_;
}


void CanvasExt::setImageOffset(const phoenix::Geometry& offset)
{
	offset_ = offset;
}


void CanvasExt::updateCanvasBuffer()
{
	//std::cout <<"Repaint: " <<image_.getSize().width <<"," <<image_.getSize().height <<" on: " <<bufferSize().width <<"," <<bufferSize().height <<" at: " <<offset_.x <<"," <<offset_.y <<"\n";

	uint32_t* buffer_ = buffer();
	const Geometry& geom = bufferSize();

	//Source and destination pointers
	const uint32_t* src = image_.getPixels();
	uint32_t* dest = buffer_;

	//Advance each pointer to the correct row.
	dest += nall::max(0, offset_.y) * geom.width;
	src += -nall::min(0, offset_.y) * image_.getSize().width;

	//Advance each pointer to the correct column
	dest += nall::max(0, offset_.x);
	src += -nall::min(0, offset_.x);

	//Calculate the effective width/height of the section to copy.
	size_t rHeight = nall::min(offset_.y+image_.getSize().height, geom.height) - nall::max(0, offset_.y);
	size_t rWidth = nall::min(offset_.x+image_.getSize().width, geom.width) - nall::max(0, offset_.x);

	//Iterate
	for (size_t row=0; row<rHeight; row++) {
		//Copy that row over
		memcpy(dest, src, rWidth*sizeof(uint32_t));

		//Increment
		dest += geom.width;
		src += image_.getSize().width;
	}
}


void CanvasExt::update()
{
	//First, copy the buffer over
	updateCanvasBuffer();

	//Now, run the regular update code
	Canvas::update();
}


