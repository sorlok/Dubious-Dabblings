#include "image-icon.hpp"

using namespace phoenix;


ImageIcon::ImageIcon() : Canvas(), img()
{
}

void ImageIcon::setImage(const nall::string& filename)
{
	//Attempt to load the image.
	if (img.decode(filename)) {
		//Not sure if transform() is required once to get data to point to the right thing.
		img.transform();
	}
}

void ImageIcon::update()
{
	//Check if we have anything to paint.
	if (img.data) {

		//Canvas shorthands
		uint32_t* buffer_ = buffer();
		const Geometry& geom = geometry();
		unsigned int srcW = img.info.width;
		unsigned int srcH = img.info.height;

		//Quick check
		if (geom.width!=srcW || geom.height!=srcH) {
			memset(buffer_, 0xFF00FF00, geom.width*geom.height);
		} else {
			//We could simply flush the entire data array at once, but I think it's
			//better to copy it row-by-row; that way, we have more flexibility later for
			//drawing, e.g., a background or for tiling the input.
			const uint32_t* src = img.data;
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



