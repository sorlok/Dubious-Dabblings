#include "PremultImage.hpp"

//NOTE: Including PNG here is buggy... it seems to be the ONLY place it can be included!
#include <nall/png.hpp>

#include <iostream>

using namespace phoenix;
using nall::file;
using nall::png;
using std::initializer_list;
using std::vector;
using std::string;


PremultImage::PremultImage() : buffer_(NULL), size_({0,0,0,0})
{

}

PremultImage::~PremultImage()
{
	if (buffer_) {
		delete [] buffer_;
		buffer_ = NULL;
	}
	if (undoBuffer_) {
		delete [] undoBuffer_;
		undoBuffer_ = NULL;
	}
}


void PremultImage::initFromBuffer(const PremultImage& sourceImg, ROTATION modify)
{
	//Our result buffer will always have the same number of pixels, just perhaps
	//   with a different width/height
	size_t w = sourceImg.getSize().width;
	size_t h = sourceImg.getSize().height;
	uint32_t* result = new uint32_t[sourceImg.getSize().width*sourceImg.getSize().height];

	//Calculate
	switch (modify) {
		case ROTATION::NONE:
			//Simple
			memcpy(result, sourceImg.buffer_, (w*h*sizeof(*result)));
			break;

		case ROTATION::FLIP_HORIZ:
		{
			//Also fairly easy; just be careful not to miss the middle column.
			uint32_t* dest = result;
			uint32_t* src = sourceImg.buffer_;
			for (size_t rowID=0; rowID<h; rowID++) {
				std::reverse_copy(src, src+w, dest);
				dest += w;
				src += w;
			}

			break;
		}
		case ROTATION::FLIP_VERT:
		{
			//Even simpler; just advance one pointer in reverse.
			//   Just make sure not to hit the middle row with both pointers at once.
			uint32_t* dest = result;
			uint32_t* src = sourceImg.buffer_ + w*(h-1);
			for (size_t rowID=0; rowID<h; rowID++) {
				memcpy(dest, src, w*sizeof(*src));
				dest += w;
				src -= w;
			}

			break;
		}

		case ROTATION::CW_90_DEG:
		{
			//A bit more complex, but only because I have a frail grasp of geometry.
			if (w != h) {
				//We should definitely fix this later.
				throw std::runtime_error("Can't rotate non-square images.");
			}

			//Make a new array, copy each row into its appropriate column
			for (int y=0; y<h; y++) {
				for (int x=0; x<w; x++) {
					int destX = (h-1-x)*w+y;
					int srcX = y*w+x;

					//Set it (flip the y axis, of course)
					//if (destX>=0 && destX<w*h && srcX>=0 && srcX<w*h) {
						result[destX] = sourceImg.buffer_[srcX];
					//}
				}
			}

			break;
		}

		default:
			throw std::runtime_error("Invalid rotatin flag.");
	}

	//Set it
	resetSize({0, 0, w, h}, result);
}



void PremultImage::initFromImage(const std::string& path)
{
	//Load it
	size_t w, h;
	uint32_t* pixels = LoadPNGFile(path, w, h);

	//Resize, set pixels
	resetSize({0, 0, w, h}, pixels);
}


bool PremultImage::resetSize_(const Geometry& size)
{
	//Delete the old one
	if (buffer_) {
		delete [] buffer_;
		buffer_ = NULL;
	}

	//Delete the undo buffer too
	if (undoBuffer_) {
		delete [] undoBuffer_;
		undoBuffer_ = NULL;
	}

	//If the new size is zero, don't create a new buffer.
	size_ = size;
	return size.width*size.height != 0;
}


void PremultImage::createUndoBuffer()
{
	size_t dim = size_.width*size_.height;
	if (dim!=0) {
		undoBuffer_ = new uint32_t[dim];
	}
}


void PremultImage::resetSize(const Geometry& size, uint32_t* newBuffer)
{
	if (!resetSize_(size)) {
		return;
	}

	std::cout <<"Resize: " <<nall::hex((unsigned int)newBuffer) <<"\n";

	//NOTE: We should probably have a way to check if newBuffer is the right size
	buffer_ = newBuffer;
}


void PremultImage::resetSize(const Geometry& size, std::vector<uint8_t> fill)
{
	if (!resetSize_(size)) {
		return;
	}

	//Get the background color:
	uint32_t color = PremultImage::DecodeColor(fill);

	std::cout <<"Resize: " <<size.width <<"," <<size.height <<"\n";

	//Allocate the new one
	size_t dim = size.width*size.height;
	buffer_ = new uint32_t[dim];

	//As fun as it might be to allocate the image with garbage
	//  pixel data, I suppose we should zero it (grumble grumble)
	std::fill_n(buffer_, dim, color);
}

const phoenix::Geometry& PremultImage::getSize() const
{
	return size_;
}

bool PremultImage::isEmpty()
{
	return size_.width==0 && size_.height==0;
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


void PremultImage::paintImage(const PremultImage& img, const Geometry& position)
{
	if (position.x<0 || position.y<0 || position.x+img.getSize().width>=getSize().width || position.y+img.getSize().height>=getSize().height) {
		//Again, need to fix, but undo buffers complicate things slightly.
		throw std::runtime_error("Error: Can't draw outside image boundaries (for now).");
	}

	//Step 1: Update the undo buffer (if any)
	const uint32_t* src = img.buffer_;
	uint32_t* dest = buffer_ + position.y*getSize().width + position.x;
	if (img.undoBuffer_) {
		const uint32_t* src2 = dest;
		uint32_t* dest2 = img.undoBuffer_;
		for (size_t rowID=0; rowID<img.getSize().height; rowID++) {
			memcpy(dest2, src2, img.getSize().width*sizeof(*src));
			src2 += getSize().width;
			dest2 += img.getSize().width;
		}
	}

	//Step 2: Paint the image.
	for (size_t rowID=0; rowID<img.getSize().height; rowID++) {
		for (size_t x=0; x<img.getSize().width; x++) {
			PremultImage::Combine(dest[x], src[x]);
		}
		src += img.getSize().width;
		dest += getSize().width;
	}
}




//////////////////////////////////////////////////////////////
// PNG loading code
//////////////////////////////////////////////////////////////

//#include "png.h"

/*namespace {
//Helper const
const unsigned int PNGSIGSIZE = 8;

//Custom PNG reader function
void custom_png_read(png_structp pngPtr, png_bytep data, png_size_t length)
{
	//Cast our IO buffer to a istream* (which ifstream is) and read 'length' bytes
	png_voidp a = png_get_io_ptr(pngPtr);
	(reinterpret_cast<std::istream*>(a))->read((char*)data, length);
}


} //End anon namespace
*/


uint32_t* PremultImage::LoadPNGFile(const string& path, unsigned int& imgWidth, unsigned int& imgHeight)
{
	//Set initial return values
	imgWidth = 0;
	imgHeight = 0;

	//Open it
	//std::ifstream source(path);
	//if (source.fail()) {
	//	return NULL;
	//}

	//Open and read in file as a bytestream.
	uint8_t *data;
	unsigned int size;
	if (!file::read(path.c_str(), data, size)) {
		return NULL;
	}

	//Decode the image, delete the old file buffer.
	png image;
	bool decodedOk = image.decode(data, size);
	delete[] data;
	//image.alphaTransform(0x40c0c0);

	//Check
	if (!decodedOk) {
		return NULL;
	}

	//Return the array of pixels
	uint32_t* res = new uint32_t[image.size];
	memcpy(res, image.data, image.size);
	return res;

	//Check the PNG signature
    /*png_byte pngsig[PNGSIGSIZE];
    source.read((char*)pngsig, PNGSIGSIZE);
    if (!source.good() || (png_sig_cmp(pngsig, 0, PNGSIGSIZE)!=0)) {
    	source.close();
    	return NULL;
    }*/

    //Create the read struct
    /*png_structp pngRead = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngRead) {
        source.close();
        return NULL;
    }

    //Create the info struct
    png_infop pngInfo = png_create_info_struct(pngRead);
    if (!pngInfo) {
    	source.close();
        png_destroy_read_struct(&pngRead, (png_infopp)0, (png_infopp)0);
        return NULL;
    }

    //Annoying "jump" error recovery, since I borrowed this code from a tutorial.
    //   TODO: Some other error recovery.
    png_bytep* rowPtrs = NULL;
    char* data = NULL;
    if (setjmp(png_jmpbuf(pngRead))) {
        //Cleanup
        png_destroy_read_struct(&pngRead, &pngInfo,(png_infopp)0);
        if (rowPtrs != NULL) { delete [] rowPtrs; }
        if (data != NULL) { delete [] data; }
        return NULL;
    }

    //Set our custom read function
    png_set_read_fn(pngRead,(voidp)&source, custom_png_read);

    //We've already read the signature, so inform libPNG to skip it.
    // Then, read the info struct.
    png_set_sig_bytes(pngRead, PNGSIGSIZE);
    png_read_info(pngRead, pngInfo);

    //Retrive some image properties
    imgWidth =  png_get_image_width(pngRead, pngInfo);
    imgHeight = png_get_image_height(pngRead, pngInfo);
    png_uint_32 bitdepth   = png_get_bit_depth(pngRead, pngInfo);
    png_uint_32 channels   = png_get_channels(pngRead, pngInfo);
    png_uint_32 color_type = png_get_color_type(pngRead, pngInfo);

    //Request palettes be converted into RGB
    if (color_type==PNG_COLOR_TYPE_PALETTE) {
    	png_set_palette_to_rgb(pngRead);
    	channels = 3;
    }

    //Request grayscale be converted to RGB
    if (color_type==PNG_COLOR_TYPE_GRAY) {
        if (bitdepth < 8) {
        	png_set_gray_1_2_4_to_8(pngRead);
        }
        bitdepth = 8;
    }

    //Request that a transparency set be converted to an alpha channel
    if (png_get_valid(pngRead, pngInfo, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(pngRead);
        channels+=1;
    }

    //Round 16-bit precision down to 8
    if (bitdepth == 16) {
    	png_set_strip_16(pngRead);
    }

    //Array of row pointers.
    rowPtrs = new png_bytep[imgHeight];

    //Allocate a buffer.
    size_t dataSize = imgWidth * imgHeight * bitdepth * channels / 8;
    data = new char[dataSize];

    //Length in bytes of one row
    const unsigned int stride = imgWidth * bitdepth * channels / 8;

    //Initialize row pointers
    for (size_t i=0; i<imgHeight; i++) {
        rowPtrs[i] = (png_bytep)data + (i * stride);
    }

    //Load all the rows one-by-one
    png_read_image(pngRead, rowPtrs);

    //Cleanup
    delete[] (png_bytep)rowPtrs;
    png_destroy_read_struct(&pngRead, &pngInfo, (png_infopp)0);
    source.close();

    //Convert results
    uint32_t* res = new uint32_t[dataSize/4];
    for (size_t i=0; i<dataSize; i+=4) {
    	//uint32_t pix = ((((uint32_t)data[i])&0xFF)<<24) | ((((uint32_t)data[i+1])&0xFF)<<16) | ((((uint32_t)data[i+2])&0xFF)<<8) | (((uint32_t)data[i+3])&0xFF);

    	//Strange... RGBA?
    	uint32_t pix = ((((uint32_t)data[i+3])&0xFF)<<24) | ((((uint32_t)data[i])&0xFF)<<16) | ((((uint32_t)data[i+1])&0xFF)<<8) | (((uint32_t)data[i+2])&0xFF);

    	res[i/4] = PremultImage::Premultiply(pix);
    }

    //Cleanup, return
    delete [] data;*/
    //return res;
}


