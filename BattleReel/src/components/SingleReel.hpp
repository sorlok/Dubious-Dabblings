#pragma once

#include <map>
#include <tuple>
#include <phoenix.hpp>
#include <nall/png.hpp>
#include "phoenix_ext/image-icon.hpp"


//A SlotImage is used to help lookup slots in a global map.
// (Kind of roundabout, I know)
struct SlotImage {
	nall::string name;
	unsigned char hexCode;
	nall::png img;
	SlotImage(const nall::string& name, unsigned char hexCode);
	SlotImage(const SlotImage& copy);

private:
	void init(const nall::string& name);
};


//
// A "SingleReel" contains several "SingleSlots"
//
struct SingleSlot {
	ImageIcon icon;
	unsigned char statusID;
	unsigned int numBP;
};


//
// The "SingleReel" class presents a "concise" representation of a battle handicap reel.
// It displays the minimal amount of information required for recognizing a given reel,
// while also saving detailed information on this reel for later retrieval.
//
class SingleReel {
public:
	unsigned int id;
	std::tuple<SingleSlot, SingleSlot, SingleSlot> slots[7];

	void loadData(unsigned char* dataStart, const std::map<unsigned int, SlotImage>& imgLookup);

private:
	static const size_t NUM_SLOTS = 8;

	void nullAll(const nall::png& defaultImg);

	//Location within the binary file, with a bounds check.
	unsigned char* dataStart;
	size_t dataSize;
};


