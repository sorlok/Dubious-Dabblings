#include <phoenix/phoenix.hpp>
#include <iostream>
#include <string>
#include <map>

#include "phoenix_ext/anchor-layout.hpp"
#include "phoenix_ext/image-icon.hpp"
#include "components/SingleReel.hpp"
#include "utilities/ChunkReader.hpp"

using std::cout;
using std::endl;
using namespace nall;
using namespace phoenix;
typedef AnchorPoint::Anchor Anchor;
AnchorPoint Centered = Axis::Centered();


const std::map<unsigned int, SlotImage> rouletteSlots = {
	{0x6, {"accessory", 0x6}},
	{0x5, {"all", 0x5}},
	{0x8, {"armor", 0x8}},
	{0x4, {"command", 0x4}},
	{0x17, {"cure", 0x17}},
	{0xE, {"frog", 0xE}},
	{0x12, {"halfhp", 0x12}},
	{0x14, {"halfhpmp", 0x14}},
	{0x13, {"halfmp", 0x13}},
	{0xA, {"halfspeed", 0xA}},
	{0x3, {"independent", 0x3}},
	{0x7, {"item", 0x7}},
	{0x11, {"level10", 0x11}},
	{0x10, {"level5", 0x10}},
	{0x16, {"lucky", 0x16}},
	{0x0, {"magic", 0x0}},
	{0xC, {"mini", 0xC}},
	{0xD, {"poison", 0xD}},
	{0x2, {"summon", 0x2}},
	{0x1, {"support", 0x1}},
	{0xF, {"time", 0xF}},
	{0x9, {"weapon", 0x9}},
	{0x15, {"zeromp", 0x15}},
	{0xFF, {"unknown", 0xFF}}
};



struct Application : Window {
  AnchorLayout layout;

  ChunkReader chReader;
  LineEdit fileName;
  Button loadFile;
  Button saveFile;
  Button quitButton;

  Button pgUp;
  Button pgDown;
  int page;

  size_t numTestReels;
  SingleReel testReels[4];

  void updateReels() {
  	int startID = page * numTestReels;
  	for (size_t i=0; i<numTestReels; i++) {
  		int id = i + startID;
  		if (id<128) { //TODO: 128 is just what the game expects; should make this more flexible.
  			testReels[i].loadData(rouletteSlots, chReader.getFile(1), id);
  		} else {
  			testReels[i].loadData(rouletteSlots);
  		}
  	}
  }

  void create() {
    //Do window tasks
    setTitle("FF7 Battle Reel Editor");
    setGeometry({ 130, 130, 650, 490 });

    //Ensure that the testReel can at least size itself (we can add images later).
    page = 0;
    numTestReels = 4;
    for (size_t i=0; i<numTestReels; i++) {
    	testReels[i].loadData(rouletteSlots);
    }
    fileName.setText("test/co.bin");
    loadFile.setText("Load File");
    saveFile.setText("Save Changes");
    saveFile.setEnabled(false);
    quitButton.setText("Quit");
    pgUp.setText("PgUp");
    pgDown.setText("PgDn");
    pgUp.setEnabled(false);
    pgDown.setEnabled(false);

    //Layout
    layout.setMargin(10);
    layout.append(loadFile, {{fileName, 10}}, {{0.0}});
    layout.append(fileName, {{0.0}, {fileName, 150, Anchor::Left}}, {Centered, {loadFile, 1}});
    layout.append(saveFile, {{loadFile, 10}}, {{loadFile, 0, Anchor::Top}});
    layout.append(quitButton, {{}, {1.0}}, {{loadFile, 0, Anchor::Top}});
    Layout* lastLayout = nullptr;
    for (size_t i=0; i<numTestReels; i++) {
    	Layout& reel = testReels[i].getLayout();
    	int suggWidth = testReels[i].getSuggestedMinimumSize().width;
    	int suggHeight = testReels[i].getSuggestedMinimumSize().height;
    	if (!lastLayout) {
    		layout.append(reel, {{0.0, 50}, {reel, suggWidth, Anchor::Left}}, {{loadFile, 20}, {reel, suggHeight, Anchor::Top}});
    	} else {
    		layout.append(reel, {{*lastLayout, 0, Anchor::Left}, {reel, suggWidth, Anchor::Left}}, {{*lastLayout}, {reel, suggHeight, Anchor::Top}});
    	}
    	lastLayout = &reel;
    }
    layout.append(pgUp, {{testReels[0].getLayout()}}, {{testReels[0].getLayout(), 5, Anchor::Top}});
    layout.append(pgDown, {{testReels[numTestReels-1].getLayout()}}, {{}, {testReels[numTestReels-1].getLayout(), -5, Anchor::Bottom}});

    //Master layout
    append(layout);


    pgDown.onTick = [&page, &numTestReels, this] {
    	//TODO: Again, maxPage is rigid here for now reason
    	int maxPage = 128/numTestReels + (128%numTestReels ? 1 : 0);
    	if (page<maxPage-1) {
    		page++;
    		updateReels();
    	}
    };

    pgUp.onTick = [&page, this] {
    	if (page>0) {
    		page--;
    		updateReels();
    	}
    };


    loadFile.onTick = [&fileName, &loadFile, &saveFile, &pgUp, &pgDown, &chReader, &testReels, &numTestReels, &page, this] {
    	fileName.setEnabled(false);
    	loadFile.setEnabled(false);

    	if (chReader.loadFile(fileName.text()()) && chReader.getNumFiles()==2) {
    		updateReels();
    	} else {
    	    for (size_t i=0; i<numTestReels; i++) {
    	    	testReels[i].loadData(rouletteSlots);
    	    }
    	}

    	fileName.setEnabled(true);
    	loadFile.setEnabled(true);
    	saveFile.setEnabled(true);
    	pgUp.setEnabled(true);
    	pgDown.setEnabled(true);
    };

    onClose = quitButton.onTick = [&testReels, &layout, &numTestReels] {
    	layout.setSkipGeomUpdates(true);
        for (size_t i=0; i<numTestReels; i++) {
        	testReels[i].getLayout().setSkipGeomUpdates(true);
        }
    	OS::quit();
    };

    setVisible();
  }
} application;


int main(int argc, char* argv[])
{
  application.create();
  OS::main();
  return 0;
}



