#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#include "phoenix_ext/PremultImage.hpp"


/**
 * Holds a physically draw-able map. No word yet on layers.
 */
class GameMap {
public:
	GameMap();

	//Helper. TODO: Returning a pointer is silly.
	static uint32_t* LoadPNGFile(const std::string& path);

	///Load a "Tiled" TMX map.
	static void InitTMXMap(GameMap& map, const std::string& path);
	//TODO: More map types


	///Use this map to paint a PremultImage
	void PaintImage(PremultImage& image);
	//TODO: Another function called "UpdateImage" which handles animations, cursors, etc.

private:
	void PaintTile(size_t tileX, size_t tileY, int tileID, PremultImage& img);

	std::vector<uint32_t*> tiles;
	std::vector< std::vector<int> > maplayer;
	unsigned int tileSize;
	phoenix::Geometry mapSizeInTiles;



};


