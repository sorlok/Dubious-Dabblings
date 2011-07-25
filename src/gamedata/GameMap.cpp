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


uint32_t* GameMap::LoadPNGFile(const string& path)
{
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
    png_uint_32 imgWidth =  png_get_image_width(pngRead, pngInfo);
    png_uint_32 imgHeight = png_get_image_height(pngRead, pngInfo);
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
    uint32_t* res = new uint32_t[dataSize/2];
    for (size_t i=0; i<dataSize; i+=2) {
    	//Premultiplied, of course!
    	res[i/2] = PremultImage::Premultiply((((uint32_t)data[i])<<16) | ((uint32_t)data[i+1]));
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

	//Load the image.
	uint32_t* pngBuffer = GameMap::LoadPNGFile(tsImage.GetSource());
	if (!pngBuffer) {
		throw std::runtime_error("Error loading PNG file.");
	}

	//Iterate over every tile; save it.
	size_t numCols = tsImage.GetWidth() / tileset.GetTileWidth();
	size_t numRows = tsImage.GetHeight() / tileset.GetTileHeight();
	for (size_t tileY=0; tileY<numRows; tileY++) {
		for (size_t tileX=0; tileX<numCols; tileX++) {
			//Prepare a new tile entry.
			map.tiles.push_back(new uint32_t[map.tileSize*map.tileSize]);

			//Get our src and dest row pointers
			uint32_t* destRow = map.tiles.back();
			uint32_t* srcRow = pngBuffer;
			srcRow += tileY*tsImage.GetWidth()*tileset.GetTileHeight();
			srcRow += tileX*tileset.GetTileWidth();

			//Copy each row of pixels into this new entry.
			for (size_t rowID=0; rowID<map.tileSize; rowID++) {
				memcpy(destRow, srcRow, map.tileSize*sizeof(*destRow));
				destRow += map.tileSize;
				srcRow += tsImage.GetWidth();
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
	int firstGID = tileset.GetFirstGid();
	//NOTE: Tiled wastes a lot of width/height on empty cell values, but oh well.
	numCols = tmxMap.GetWidth();   // / tileset.GetTileWidth();
	numRows = tmxMap.GetHeight();  // / tileset.GetTileHeight();
	map.mapSizeInTiles = {0, 0, numCols, numRows};
	map.maplayer.insert(map.maplayer.begin(), numRows, vector<int>());
	for (size_t tileY=0; tileY<numCols; tileY++) {
		map.maplayer[tileY].insert(map.maplayer[tileY].begin(), numCols, -1);
		for (size_t tileX=0; tileX<numRows; tileX++) {
			map.maplayer[tileY][tileX] = layer.GetTile(tileX, tileY).gid - firstGID;
		}
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
				std::cout <<"   Tile[" <<tileX <<"," <<tileY <<" : " <<maplayer[tileY][tileX] <<"\n";
				PaintTile(tileX, tileY, maplayer[tileY][tileX], image);
			}
		}
	}
}






