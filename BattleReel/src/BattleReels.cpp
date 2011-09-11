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
	{0, {"accessory", 0}},
	{1, {"all", 1}},
	{2, {"armor", 2}},
	{3, {"command", 3}},
	{4, {"cure", 4}},
	{5, {"frog", 5}},
	{6, {"halfhp", 6}},
	{7, {"halfhpmp", 7}},
	{8, {"halfmp", 8}},
	{9, {"halfspeed", 9}},
	{10, {"independent", 10}},
	{11, {"item", 11}},
	{12, {"level10", 12}},
	{13, {"level5", 13}},
	{14, {"lucky", 14}},
	{15, {"magic", 15}},
	{16, {"mini", 16}},
	{17, {"poison", 17}},
	{18, {"summon", 18}},
	{19, {"support", 19}},
	{20, {"time", 20}},
	{21, {"weapon", 21}},
	{22, {"zeromp", 22}},
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

  size_t numTestReels;
  SingleReel testReels[4];

  void create() {
    //Do window tasks
    setTitle("FF7 Battle Reel Editor");
    setGeometry({ 130, 130, 650, 490 });

    //Ensure that the testReel can at least size itself (we can add images later).
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

    loadFile.onTick = [&fileName, &loadFile, &saveFile, &chReader, &testReels, &numTestReels] {
    	fileName.setEnabled(false);
    	loadFile.setEnabled(false);

    	if (chReader.loadFile(fileName.text()()) && chReader.getNumFiles()==2) {
    		//Update reels
    	    for (size_t i=0; i<numTestReels; i++) {
    	    	testReels[i].loadData(rouletteSlots, chReader.getFile(1), i);
    	    }
    	} else {
    	    for (size_t i=0; i<numTestReels; i++) {
    	    	testReels[i].loadData(rouletteSlots);
    	    }
    	}

    	fileName.setEnabled(true);
    	loadFile.setEnabled(true);
    	saveFile.setEnabled(true);
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



