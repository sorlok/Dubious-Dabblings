#include "image-icon.hpp"

#include <iostream>

using namespace phoenix;


ImageIcon::ImageIcon() : Canvas(), img(nullptr)
{
}

void ImageIcon::setImage(const nall::png& image)
{
	img = &image;
	Canvas::setGeometry(minimumGeometry());
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
	//Check if we have anything to paint.
	if (img->data) {
		//Canvas shorthands
		uint32_t* buffer_ = buffer();
		const Geometry& geom = geometry();
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
			const uint32_t* src = img->data;
			uint32_t* dest = buffer_;

			//Iterate & copy
			for (size_t row=0; row<srcH; row++) {
				//Copy that row over
				memcpy(dest, src, srcW*sizeof(uint32_t));

				//Increment
				dest += geom.width;
				src += srcW;
			}
		}
	}

	//Delegate to superclass
	Canvas::update();
}



