#pragma once

#include <map>
#include <tuple>
#include <string>
#include <sstream>
#include <phoenix.hpp>
#include <nall/png.hpp>
#include "phoenix_ext/image-icon.hpp"
#include "phoenix_ext/anchor-layout.hpp"


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

	SingleReel() : layoutDone(false) {
		numLbl.setFont({"Arial, 14, bold"});
	}
	void loadData(const std::map<unsigned int, SlotImage>& imgLookup, unsigned char* dataStart=nullptr, unsigned int reelID=0);
	AnchorLayout& getLayout();
	phoenix::Geometry getSuggestedMinimumSize();

private:
	static const size_t NUM_SLOTS = 7;

	//static phoenix::Font lblFont;

	void nullAll(const nall::png& defaultImg);

	//For drawing
	bool layoutDone;
	AnchorLayout layout;
	phoenix::Label numLbl;

	//Location within the binary file, with a bounds check.
	unsigned char* dataStart;
	size_t dataSize;
};


