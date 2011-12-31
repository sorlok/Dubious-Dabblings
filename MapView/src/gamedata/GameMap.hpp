//#pragma once
#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include "GenConfig.h"

#include <cstdint>
#include <vector>

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
#ifndef BUILD_SHARED_LIBRARY
	static void InitTMXMap(GameMap& map, const std::string& path);
	//TODO: More map types


	///Use this map to paint a PremultImage
	void PaintImage(PremultImage& image);
	//TODO: Another function called "UpdateImage" which handles animations, cursors, etc.

//private: //TODO: Better encapsulation
	void PaintTile(size_t tileX, size_t tileY, int tileID, PremultImage& img);
#endif

	std::vector<uint32_t*> tiles;
	std::vector< std::vector<int> > maplayer;
	unsigned int tileSize;

#ifndef BUILD_SHARED_LIBRARY
	phoenix::Geometry mapSizeInTiles;
#endif




};


#endif
