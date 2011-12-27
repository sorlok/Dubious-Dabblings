//Copyright 2011 by Seth N. Hetu
//Shared under the terms of the ISC license

#include "image-icon.hpp"

#include <iostream>

using namespace phoenix;


ImageIcon::ImageIcon() : Canvas(), img(nullptr), bkgrd(0)
{
}

void ImageIcon::setImage(const nall::png& image)
{
	//Requires a little bit of thought to avoid messing with layouts
	img = &image;
	Geometry old = geometry();
	Geometry min = minimumGeometry();

	//Save and update
	Canvas::setGeometry({old.x, old.y, min.width, min.height});
	update();
}


phoenix::Geometry ImageIcon::minimumGeometry()
{
	if (img) {
		return {0, 0, img->info.width, img->info.height};
	}
	return Canvas::minimumGeometry();
}

void ImageIcon::setGeometry(const phoenix::Geometry &geometry)
{
	//NOTE: ImageIcons always have the same geometry.
	Canvas::setGeometry({geometry.x, geometry.y, minimumGeometry().width, minimumGeometry().height});
	//update();  //May need to uncomment this if canvas buffers start disappearing.
}


void ImageIcon::update()
{
	//First, fill it
	uint32_t* buffer_ = data();
	const Geometry& geom = geometry();
	std::fill(buffer_, buffer_+geom.width*geom.height, bkgrd);

	//Check if we have anything to paint.
	if (img->data) {
		//Canvas shorthands
		unsigned int srcW = img->info.width;
		unsigned int srcH = img->info.height;

		//Quick check
		if (geom.width!=srcW || geom.height!=srcH) {
			std::cout <<"Err: " <<geom.width <<"," <<geom.height <<"\n";
			std::fill(buffer_, buffer_+geom.width*geom.height, 0xFF00FF00);
		} else {
			//We could simply flush the entire data array at once, but I think it's
			//better to copy it row-by-row; that way, we have more flexibility later for
			//drawing, e.g., a background or for tiling the input.
			const uint8_t* src = img->data;
			const unsigned bpp = img->info.bytesPerPixel;
			uint32_t* dest = buffer_;

			//Iterate & copy
			for (size_t row=0; row<srcH; row++) {
				//Copy that row over
				//TODO: This should really pain ONTO the background color
				//memcpy(dest, src, srcW*sizeof(uint32_t));
				//TEMP fix: we can do this better later.
				for (size_t col=0; col<srcW; row++) {
					*dest = 0;
					for (size_t bit=0; bit<bpp; bit++) {
						*dest = ((*dest)<<8) | *src;
						src++;
					}
					dest++;
				}

				//Increment
				//dest += geom.width;
				//src += srcW*bpp;
			}
		}
	}

	//Delegate to superclass
	Canvas::update();
}



