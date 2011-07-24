#include "CanvasExt.hpp"

#include <iostream>

using namespace phoenix;


CanvasExt::CanvasExt() : Canvas(), image_(), offset_({0,0,0,0})
{
}



PremultImage& CanvasExt::getBufferedImage()
{
	//Quick check
	//if (getGeometry().width!=image_.getSize().width || getGeometry().height!=image_.getSize().height) {
	//	image_.resetSize(getGeometry());
	//}  //Noooooooooooooo

	return image_;
}


void CanvasExt::setImageOffset(const phoenix::Geometry& offset)
{
	offset_ = offset;
}


void CanvasExt::updateCanvasBuffer()
{
	//std::cout <<"Updating canvas buffer\n";

	uint32_t* buffer_ = buffer();
	const Geometry& geom = bufferSize();

	//TODO: Totally need to hack this with memcpy
	for (int srcY=0; srcY<image_.getSize().height; srcY++) {
		for (int srcX=0; srcX<image_.getSize().width; srcX++) {
			int destY = srcY + offset_.y;
			int destX = srcX + offset_.x;
			if (destX>=0 && destY>=0 && destX<geom.width && destY<geom.height) {  //How did that not crash before!
				uint32_t srcVal = image_[srcY*image_.getSize().width+srcX];
				buffer_[destY*geom.width + destX] = srcVal;   //SIGSEGV here on resizing.
			}
		}
	}


}


void CanvasExt::update()
{
	//Check
	//Seems to be needed or dragging the window resets the buffer.
	//if (cached_size_.width!=getGeometry().width || cached_size_.height!=getGeometry().height) {
		//cached_size_ = getGeometry();

		//First, copy the buffer over
		updateCanvasBuffer();

		//Now, run the regular update code
		Canvas::update();
	//}
}


