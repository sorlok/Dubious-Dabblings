//#pragma once
#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <vector>

#include "phoenix_ext/PremultImage.hpp"


/**
 * Holds a physically draw-able map. No word yet on layers.
 */
class GameMap {
public:
	GameMap();

	///Load a "Tiled" TMX map.
	static void InitTMXMap(GameMap& map, const std::string& path);
	//TODO: More map types


	///Use this map to paint a PremultImage
	void PaintImage(PremultImage& image);
	//TODO: Another function called "UpdateImage" which handles animations, cursors, etc.

//private: //TODO: Better encapsulation
	void PaintTile(size_t tileX, size_t tileY, int tileID, PremultImage& img);

	std::vector<uint32_t*> tiles;
	std::vector< std::vector<int> > maplayer;
	unsigned int tileSize;
	phoenix::Geometry mapSizeInTiles;




};


#endif
