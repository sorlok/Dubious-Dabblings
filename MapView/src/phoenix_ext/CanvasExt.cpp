#include "CanvasExt.hpp"

#include <iostream>

using namespace phoenix;


CanvasExt::CanvasExt() : Canvas(), image_(), offset_(0,0,0,0), lastW(0), lastH(0)
{
}



PremultImage& CanvasExt::getBufferedImage()
{
	return image_;
}

bool CanvasExt::needsResize()
{
	return lastW==0 || lastH==0 || lastW!=geometry().width || lastH!=geometry().height;
}

void CanvasExt::setImageOffset(const phoenix::Geometry& offset)
{
	offset_ = offset;
}

/*void CanvasExt::loadArrowMarkings(const std::string& filename)
{
	//Right, Left
	arrows[0].initFromImage(filename);
	arrows[2].initFromBuffer(arrows[0], ROTATION::FLIP_HORIZ);

	//Up, Down
	arrows[1].initFromBuffer(arrows[0], ROTATION::CW_90_DEG);
	arrows[3].initFromBuffer(arrows[1], ROTATION::FLIP_VERT);

	//Undo buffers for all!
	for (size_t i=0; i<4; i++) {
		arrows[i].createUndoBuffer();
	}
}*/


/*void CanvasExt::paintArrows()
{
	//Calculate coordinates of the rectangles to center each arrow in.
	size_t w = geometry().width;
	size_t h = geometry().height;
	Geometry arrowRects[4];
	arrowRects[0] = {3*w/4, 0, w/4, h};  //Right
	arrowRects[2] = {0, 0, w/4, h};  //Left
	arrowRects[1] = {0, 0, w, h/4};  //Up
	arrowRects[3] = {0, 3*h/4, w, h/4};  //Down

	//Now, do it.
	for (size_t i=0; i<4; i++) {
		//Adjust
		Geometry& rect = arrowRects[i];
		PremultImage& arrow = arrows[i];
		rect.x += rect.width/2 - arrow.getSize().width/2;
		rect.y += rect.height/2 - arrow.getSize().height/2;
		rect.width = arrow.getSize().width;
		rect.height = arrow.getSize().height;

		//Paint
		image_.paintImage(arrow, rect);
	}
}*/


void CanvasExt::updateCanvasBuffer()
{
	//std::cout <<"Repaint: " <<image_.getSize().width <<"," <<image_.getSize().height <<" on: " <<bufferSize().width <<"," <<bufferSize().height <<" at: " <<offset_.x <<"," <<offset_.y <<"\n";

	uint32_t* buffer_ = data();
	//const Geometry& geom = geometry();
	const Size& geom = size(); //NOTE: Why is this different than geometry()?
	lastW = geom.width;
	lastH = geom.height;

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

	std::cout <<"Offset: " <<offset_.x <<"\n";
	std::cout <<"Image width: " <<image_.getSize().width <<"\n";
	std::cout <<"Geom width: " <<geom.width <<"\n";
	std::cout <<"Buffer size: " <<size().width <<"," <<size().height <<"\n";

	//Iterate
	for (size_t row=0; row<rHeight; row++) {
		std::cout <<"Row: " <<row <<" of width: " <<rWidth <<std::endl;

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


