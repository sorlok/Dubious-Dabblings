#include "GameMap.hpp"

#include <algorithm>

#include "tmx/Tmx.h"

using std::string;
using std::vector;


GameMap::GameMap()
{

}


#ifndef BUILD_SHARED_LIBRARY
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
	uint32_t* pngBuffer = PremultImage::LoadPNGFile(tmxMap.GetFilepath() + "/" + tsImage.GetSource(), tsImgWidth, tsImgHeight);
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
	//std::cout <<"Test: " <<tileID <<" of " <<tiles.size() <<"\n";

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
}
#endif





