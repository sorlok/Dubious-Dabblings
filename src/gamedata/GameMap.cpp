#include "GameMap.hpp"

#include <algorithm>

#include "tmx/Tmx.h"
#include "png.h"

using std::string;
using std::vector;


GameMap::GameMap()
{

}

namespace {
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


uint32_t* GameMap::LoadPNGFile(const string& path, unsigned int& imgWidth, unsigned int& imgHeight)
{
	//Set initial return values
	imgWidth = 0;
	imgHeight = 0;

	//Open it
	std::ifstream source(path);
	if (source.fail()) {
		return NULL;
	}

	//Check the PNG signature
    png_byte pngsig[PNGSIGSIZE];
    source.read((char*)pngsig, PNGSIGSIZE);
    if (!source.good() || (png_sig_cmp(pngsig, 0, PNGSIGSIZE)!=0)) {
    	source.close();
    	return NULL;
    }

    //Create the read struct
    png_structp pngRead = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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
    delete [] data;
    return res;
}


void GameMap::InitTMXMap(GameMap& map, const std::string& path)
{
	//Read it
	Tmx::Map tmxMap;
	tmxMap.ParseFile("map_test.tmx");
	if (tmxMap.HasError()) {
		throw std::runtime_error("Error loading map.");
	}

	//Some tests; these will all be dealt with more gracefully later.
	if (tmxMap.GetNumTilesets()!=1) {throw std::runtime_error("Maps must only have one tileset.");}
	if (tmxMap.GetNumLayers()!=1) {throw std::runtime_error("Maps must only have one layer.");}
	if (tmxMap.GetNumObjectGroups()!=0) {throw std::runtime_error("Maps must have no objects.");}
	if (tmxMap.GetOrientation()!=Tmx::TMX_MO_ORTHOGONAL) {throw std::runtime_error("Only orthogonal maps supported.");}
	if (tmxMap.GetTileWidth()!=tmxMap.GetTileHeight()) {throw std::runtime_error("Tile width must be equal to tile height.");}

	//Set
	map.tileSize = tmxMap.GetTileWidth();

	//Get the only tileset; iterate through it.
	const Tmx::Tileset& tileset = *(tmxMap.GetTileset(0));
	const Tmx::Image& tsImage = *(tileset.GetImage());

	//Check the name
	string tsetName = tsImage.GetSource();
	std::transform(tsetName.begin(), tsetName.end(), tsetName.begin(), ::tolower);
	if (tsetName.rfind("png") != tsetName.size()-3) {throw std::runtime_error("Tileset must be a PNG image.");}
	if (tileset.GetMargin()!=0) { throw std::runtime_error("Tilesets may not have a margin."); }

	//In case they're not explicitly set:
	unsigned int tsImgWidth;
	unsigned int tsImgHeight;

	//Load the image.
	uint32_t* pngBuffer = GameMap::LoadPNGFile(tmxMap.GetFilepath() + "/" + tsImage.GetSource(), tsImgWidth, tsImgHeight);
	if (!pngBuffer) {
		throw std::runtime_error("Error loading PNG file.");
	}

	//Iterate over every tile; save it.
	size_t numCols = tsImgWidth / tileset.GetTileWidth();
	size_t numRows = tsImgHeight / tileset.GetTileHeight();

	std::cout <<"Num tiles: " <<tsImgWidth <<" X " <<tileset.GetTileWidth() <<"\n";

	for (size_t tileY=0; tileY<numRows; tileY++) {
		for (size_t tileX=0; tileX<numCols; tileX++) {
			//Prepare a new tile entry.
			map.tiles.push_back(new uint32_t[map.tileSize*map.tileSize]);

			//Get our src and dest row pointers
			uint32_t* destRow = map.tiles.back();
			uint32_t* srcRow = pngBuffer;
			srcRow += tileY*tsImgWidth*tileset.GetTileHeight();
			srcRow += tileX*tileset.GetTileWidth();

			//Copy each row of pixels into this new entry.
			for (size_t rowID=0; rowID<map.tileSize; rowID++) {
				memcpy(destRow, srcRow, map.tileSize*sizeof(*destRow));
				destRow += map.tileSize;
				srcRow += tsImgWidth;
			}
		}
	}

	//Recover some memory.
	if (pngBuffer) {
		delete [] pngBuffer;
	}

	//Double-check our layer.
	const Tmx::Layer& layer = *(tmxMap.GetLayer(0));
	if (layer.GetWidth()!=tmxMap.GetWidth() || layer.GetHeight()!=tmxMap.GetHeight()) {
		throw std::runtime_error("Map layer dimensions must match map dimensions.");
	}

	//Iterate through the map. Convert the GIDs to zero-offset variants
	// NOTE: negative numbers (esp. -1) mean "no tile".
	size_t firstGID = tileset.GetFirstGid();

	//NOTE: Tiled wastes a lot of width/height on empty cell values, but oh well.
	map.mapSizeInTiles = {0, 0, tmxMap.GetWidth(), tmxMap.GetHeight()};
	for (size_t tileY=0; tileY<(size_t)tmxMap.GetHeight(); tileY++) {
		vector<int> thisRow;

		//map.maplayer[tileY].resize(numCols, -1);
		for (size_t tileX=0; tileX<(size_t)tmxMap.GetWidth(); tileX++) {
			size_t tileGID = layer.GetTile(tileX, tileY).gid;
			if (tileGID >= firstGID) {
				thisRow.push_back(tileGID - firstGID);
			}
		}

		map.maplayer.push_back(thisRow);
	}


	std::cout <<"TMX Map Loaded: " <<path <<"\n";

}


//Helper: paint a tile
void GameMap::PaintTile(size_t tileX, size_t tileY, int tileID, PremultImage& img)
{
	//Get the destination location
	uint32_t* dest = img.getPixels() + (1+tileY*(tileSize+1))*img.getSize().width + (1+tileX*(tileSize+1));
	const uint32_t* src = tiles[tileID];
	for (size_t rowID=0; rowID<tileSize; rowID++) {
		memcpy(dest, src, tileSize*sizeof(*src));
		src += tileSize;
		dest += img.getSize().width;
	}
}


void GameMap::PaintImage(PremultImage& image)
{
	//Time to generate! Wheee~~~~
	size_t tileW = mapSizeInTiles.width;
	size_t tileH = mapSizeInTiles.height;
	std::cout <<"Size in tiles: " <<tileW <<"," <<tileH <<"  of size: " <<tileSize <<"\n";
	image.resetSize({0, 0, tileW*(tileSize+1)+1, tileH*(tileSize+1)+1});

	//Tiles have a 1pixel black border on all sides. Negative indexed tiles are not drawn.
	for (size_t tileY=0; tileY<tileH; tileY++) {
		for (size_t tileX=0; tileX<tileW; tileX++) {
			if (maplayer[tileY][tileX]>=0) {
				PaintTile(tileX, tileY, maplayer[tileY][tileX], image);
			}
		}
	}



	{
		///TEST
		size_t aWidth, aHeight;
		uint32_t* pngBuffer2 = GameMap::LoadPNGFile("arrow.png", aWidth, aHeight);
		if (!pngBuffer2) {
			throw std::runtime_error("Error loading arrow PNG file.");
		}

		const uint32_t* src = pngBuffer2;
		uint32_t* dest = image.getPixels() + (image.getSize().height/2)*image.getSize().width + image.getSize().width/2;
		for (size_t rowID=0; rowID<aHeight; rowID++) {
			for (size_t x=0; x<aWidth; x++) {
				std::cout <<"Combining: " <<nall::hex(dest[x]) <<" with " <<nall::hex(src[x]) <<"\n";
				PremultImage::Combine(dest[x], src[x]); //Need to combine, or we don't get transparency.
				std::cout <<"   res: " <<nall::hex(dest[x]) <<"\n";
			}
			//memcpy(dest, src, aWidth*sizeof(*src));
			src += aWidth;
			dest += image.getSize().width;
		}
	}
}






