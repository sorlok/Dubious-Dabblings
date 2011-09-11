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



SlotImage::SlotImage(const nall::string& name, unsigned char hexCode, unsigned int comboID) : name(name), hexCode(hexCode), comboID(comboID), img()
{
	init(name);
}

SlotImage::SlotImage(const SlotImage& copy) : name(copy.name), hexCode(copy.hexCode), comboID(copy.comboID), img()
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


phoenix::Geometry SingleReel::getSuggestedMinimumSize()
{
	//TODO: These values at least should be somewhat more.... er... static.
	int margin = 5;
	int spacing = 5;
	int textW = digit1.minimumGeometry().width;
	phoenix::Geometry square = std::get<0>(slots[0]).icon.minimumGeometry();

	unsigned int w = margin*2 + textW + spacing + square.width*NUM_SLOTS + spacing*(NUM_SLOTS-1);
	unsigned int h = margin*2 + square.height;
	return {0, 0, w, h};
}


AnchorLayout& SingleReel::getLayout()
{
	typedef AnchorPoint::Anchor Anchor;
	AnchorPoint Centered = Axis::Centered(); //Makes things look cleaner.

	//Perform layout of visible components.
	if (!layoutDone) {
		layoutDone = true;

		layout.setMargin(5);
		layout.setSkipGeomUpdates(true);

		//Attach right-to-left, for no reason in particular
		phoenix::Sizable* last = nullptr;
		for (int i=NUM_SLOTS-1; i>=0; i--) {
			ImageIcon& icn = std::get<1>(slots[i]).icon;
			if (last) {
				//Test appending to the center; shouldn't do anything if components are the same height
				layout.append(icn, {{}, {*last, -5}}, {Centered, {*last}});
			} else {
				layout.append(icn, {{}, {1.0, -5}}, {{0.0}});
			}
			last = &icn;
		}

		//Test: out-of-order appending
		int shim = 3; //Courier's a tall font
		layout.append(digit1, {Centered, {digit2}}, {{}, {digit2, shim}});
		layout.append(digit2, {{}, {*last, -5}}, {Centered, {*last}});
		layout.append(digit3, {Centered, {digit2}}, {{digit2, -shim}});


		layout.setSkipGeomUpdates(false);
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
	digit1.setText("?");
	digit2.setText("?");
	digit3.setText("?");
}


//Load data for this SingleReel using a pointer to the data's start location
void SingleReel::loadData(const map<unsigned int, SlotImage>& imgLookup, unsigned char* dataStart, unsigned int reelID)
{
	//Save
	this->dataSize = NUM_SLOTS * 4 * 3;
	this->dataStart = dataStart + dataSize*reelID;
	unsigned char* src = this->dataStart;

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

				if (imgLookup.count(curr.statusID)==0) {
					std::cout <<"slotID: " <<slot <<" icon: " <<(int)curr.statusID <<" BP: " <<curr.numBP <<"\n";
				}
			}
		}

		//The number
		std::stringstream num;
		num <<reelID;
		std::string digits = PadString(num.str(), 3);
		digit1.setText(std::string(1, digits[0]).c_str());
		digit2.setText(std::string(1, digits[1]).c_str());
		digit3.setText(std::string(1, digits[2]).c_str());

		//Check
		if (src != this->dataStart+this->dataSize) {
			std::cout <<"Alignment error.\n";
		}
	}

	//Finally, update the layout, lest we end up stacking all items in the corner again.
	//layout.setGeometry(getSuggestedMinimumSize());
}



