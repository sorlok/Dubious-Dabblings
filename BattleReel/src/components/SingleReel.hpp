#pragma once

#include <map>
#include <tuple>
#include <string>
#include <sstream>
#include <functional>
#include <phoenix.hpp>
#include <nall/png.hpp>
#include "phoenix_ext/image-icon.hpp"
#include "phoenix_ext/anchor-layout.hpp"


//A SlotImage is used to help lookup slots in a global map.
// (Kind of roundabout, I know)
struct SlotImage {
	nall::string name;
	unsigned char hexCode;
	unsigned int comboID;
	nall::png img;
	SlotImage(const nall::string& name, unsigned char hexCode, unsigned int comboID);
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
		digit1.setFont({"Courier New, 14, bold"});
		digit2.setFont({"Courier New, 14, bold"});
		digit3.setFont({"Courier New, 14, bold"});

		//Add click events
		for (int i=NUM_SLOTS-1; i>=0; i--) {
			ImageIcon& icn = std::get<1>(slots[i]).icon;
			icn.onMotion = [this](unsigned int x, unsigned int y, phoenix::MOVE_FLAG moveFlag) {
				if (moveFlag==phoenix::MOVE_FLAG::LEFT_UP) {
					if (callback) {
						callback();
					}
				}
			};
		}
	}
	void loadData(const std::map<unsigned int, SlotImage>& imgLookup, unsigned char* dataStart=nullptr, unsigned int reelID=0);
	void setClickCallback(std::function<void ()> callback) { this->callback = callback; }
	AnchorLayout& getLayout();
	phoenix::Geometry getSuggestedMinimumSize();

private:
	static const size_t NUM_SLOTS = 7;

	std::function<void ()> callback;

	void nullAll(const nall::png& defaultImg);

	//For drawing
	bool layoutDone;
	AnchorLayout layout;
	phoenix::Label digit1;
	phoenix::Label digit2;
	phoenix::Label digit3;

	//Location within the binary file, with a bounds check.
	unsigned char* dataStart;
	size_t dataSize;
};


