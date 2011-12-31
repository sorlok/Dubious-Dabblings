//#pragma once
#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include "GenConfig.h"

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string>

#ifndef BUILD_SHARED_LIBRARY
#include "phoenix_ext/PremultImage.hpp"
#endif


/**
 * Holds a physically draw-able map. No word yet on layers.
 */
class GameMap {
public:
	GameMap();

	///Load a "Tiled" TMX map.
	static void InitTMXMap(GameMap& map, const std::string& path);
	//TODO: More map types

#ifndef BUILD_SHARED_LIBRARY
	///Use this map to paint a PremultImage
	void PaintImage(PremultImage& image);
	//TODO: Another function called "UpdateImage" which handles animations, cursors, etc.

//private: //TODO: Better encapsulation
	void PaintTile(size_t tileX, size_t tileY, int tileID, PremultImage& img);

	//???
	std::vector<uint32_t*> tiles;
	std::vector< std::vector<int> > maplayer;
#endif

	unsigned int tileSize;
	std::string tilePalettePath;

#ifndef BUILD_SHARED_LIBRARY
	phoenix::Geometry mapSizeInTiles;
#endif




};


#endif
