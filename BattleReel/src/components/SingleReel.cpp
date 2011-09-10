#include "SingleReel.hpp"

#include <iostream>

using std::map;
using std::string;

namespace {
//Attempt to load an image.
bool LoadPng(const nall::string& filename, nall::png& res) {
	if (res.decode(filename)) {
		//Not sure if transform() is required once to get data to point to the right thing.
		res.transform();
		return true;
	}
	return false;
}

const nall::png& GetIcon(const std::map<unsigned int, SlotImage>& imgLookup, const nall::png& defaultImg, unsigned char iconCode)
{
	if (imgLookup.count(iconCode)>0) {
		return imgLookup.find(iconCode)->second.img;
	}
	return defaultImg;
}

string PadString(const string& src, size_t len)
{
	if (src.length()>len) {
		return string(len, '#');
	}
	return string(len-src.length(),'0') + src;
}
} //End anon namespace



SlotImage::SlotImage(const nall::string& name, unsigned char hexCode) : name(name), hexCode(hexCode), img()
{
	init(name);
}

SlotImage::SlotImage(const SlotImage& copy) : name(copy.name), hexCode(copy.hexCode), img()
{
	//It's expensive to reload the PNG each time, but phoenix::png doesn't declare a move constructor,
	//   so pointers will be deleted twice otherwise.
	init(name);
}

void SlotImage::init(const nall::string& name)
{
	if (!LoadPng({"slots/", name, ".png"}, img)) {
		std::cout <<"Can't find image: " <<name <<std::endl;
	}
}


AttachLayout& SingleReel::getLayout()
{
	//Perform layout of visible components.
	if (!layoutDone) {
		layoutDone = true;

		layout.setMargin(5);
		layout.append(numLbl, {0.0}, {0.0});

		//Attach right-to-left, so that we can easily overlap the numLbl if we want to.
		phoenix::Sizable* last = nullptr;
		for (int i=NUM_SLOTS-1; i>=0; i--) {
			ImageIcon& icn = std::get<1>(slots[i]).icon;
			if (last) {
				//Test appending to the center; shouldn't do anything if components are the same height
				layout.append(icn, {}, {Attachment::SPECIAL::CENTERED, *last}, {*last, -5});
			} else {
				layout.append(icn, {}, {numLbl, 0, Attachment::ANCHOR::CENTER}, {1.0, -5});
			}
			last = &icn;
		}
	}

	return layout;
}


void SingleReel::nullAll(const nall::png& defaultImg)
{
	//We need to set all Icons here so that our layout manager works properly
	for (size_t i=0; i<NUM_SLOTS; i++) {
		std::get<0>(slots[i]).icon.setImage(defaultImg);
		std::get<1>(slots[i]).icon.setImage(defaultImg);
		std::get<2>(slots[i]).icon.setImage(defaultImg);
	}
	numLbl.setText("N/A");
}


//Load data for this SingleReel using a pointer to the data's start location
void SingleReel::loadData(const map<unsigned int, SlotImage>& imgLookup, unsigned char* dataStart, unsigned int reelID)
{
	//Save
	unsigned char* src = dataStart;
	this->dataStart = dataStart;
	this->dataSize = NUM_SLOTS * 4 * 3;

	//Retrieve a good default
	if (imgLookup.count(0xFF)==0) {
		std::cout <<"Image lookup contains no default (0xFF)." <<std::endl;
	}
	const nall::png& defaultImg = imgLookup.find(0xFF)->second.img;

	//Are we actually doing anything?
	if (!dataStart) {
		nullAll(defaultImg);
	} else {
		//Now load
		for (size_t row=0; row<3; row++) {
			for (size_t slot=0; slot<NUM_SLOTS; slot++) {
				//Something like this:
				unsigned char statusID = src[3];
				unsigned int bpReward = (src[2]<<16) | (src[1]<<8) | src[0];
				SingleSlot& curr = (row==0)?std::get<0>(slots[slot]):(row==1)?std::get<1>(slots[slot]):std::get<2>(slots[slot]);
				curr.statusID = statusID;
				curr.numBP = bpReward;
				curr.icon.setImage(GetIcon(imgLookup, defaultImg, statusID));
				src += 4;
			}
		}

		//The number
		std::stringstream num;
		num <<reelID;
		numLbl.setText(PadString(num.str(), 3).c_str());

		//Check
		if (src != dataStart+this->dataSize) {
			std::cout <<"Alignment error.\n";
		}
	}
}



