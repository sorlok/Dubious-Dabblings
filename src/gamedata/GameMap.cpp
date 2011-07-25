#include "GameMap.hpp"

#include <algorithm>

#include "tmx/TmxMap.h"

using std::string;


GameMap::GameMap()
{

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
	if (tmxMap.GetOrientation()!=TMX_MO_ORTHOGONAL) {throw std::runtime_error("Only orthogonal maps supported.");}
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
	//TODO

	//Iterate over every tile; save it.
	int firstGID = tileset.GetFirstGid();
	size_t numCols = tsImage.GetWidth() / tileset.GetTileWidth();
	size_t numRows = tsImage.GetHeight() / tileset.GetTileHeight();
	for (size_t tileY=0; tileY<numrows; tileY++) {
		for (size_t tileX=0; tileX<numcols; tileX++) {
			//Prepare a new tile entry.
			map.tiles.push_back(new uint32_t[map.tileSize*map.tileSize]);

			//Copy each row of pixels into this new entry.
			uint32_t* destRow = map.tiles.back();
			uint32_t* srcRow =
		}
	}


	//Get the only layer
	const Tmx::Layer& layer = *(tmxMap.GetLayer(0));




}

void GameMap::PaintImage(PremultImage& image)
{

}



