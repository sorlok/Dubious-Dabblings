//Test; comment out entire source code to try it.
//#include "FileManager.hpp"

//Main code; comment out if you want to use one of the test cases.
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
	{0x6, {"accessory", 0x6, 0}},
	{0x5, {"all", 0x5, 1}},
	{0x8, {"armor", 0x8, 2}},
	{0x4, {"command", 0x4, 3}},
	{0x17, {"cure", 0x17, 4}},
	{0xE, {"frog", 0xE, 5}},
	{0x12, {"halfhp", 0x12, 6}},
	{0x14, {"halfhpmp", 0x14, 7}},
	{0x13, {"halfmp", 0x13, 8}},
	{0xA, {"halfspeed", 0xA, 9}},
	{0x3, {"independent", 0x3, 10}},
	{0x7, {"item", 0x7, 11}},
	{0x11, {"level10", 0x11, 12}},
	{0x10, {"level5", 0x10, 13}},
	{0x16, {"lucky", 0x16, 14}},
	{0x0, {"magic", 0x0, 15}},
	{0xC, {"mini", 0xC, 16}},
	{0xD, {"poison", 0xD, 17}},
	{0x2, {"summon", 0x2, 18}},
	{0x1, {"support", 0x1, 19}},
	{0xF, {"time", 0xF, 20}},
	{0x9, {"weapon", 0x9, 21}},
	{0x15, {"zeromp", 0x15, 22}},
	{0xFF, {"unknown", 0xFF, 23}}
};

const std::vector<nall::string> comboLabels = {
	"Accessory", "All Materia", "Armor", "Command",
	"Full Cure", "Frog", "Half HP", "Half HP/MP",
	"Half MP", "Half Speed", "Independent", "Item",
	"Level -10", "Level -5", "Lucky 7!", "Magic",
	"Mini", "Poison", "Summon", "Support", "Time",
	"Weapon", "Zero MP", "Unknown"
};



struct Application : Window {
  struct MsgBox : Window {
	AnchorLayout layout;
	Label lbl;
	Button ok;

	void create() {
		setTitle("Nice Try");
		setGeometry({200, 200, 180, 75});

		lbl.setText("Sorry, not a chance.");
		ok.setText("Ok");

		//ScopedLayoutLock lock(&layout);
		layout.setMargin(5);
		layout.append(lbl, {Centered, {0.5}}, {Centered, {0.5, -10}});
		layout.append(ok, {Centered, {0.5}}, {{lbl, 5}});

		ok.onTick = [this] {
			setVisible(false);
		};

		append(layout);
	}
  } MsgBox;

  AnchorLayout layout;

  ChunkReader chReader;
  LineEdit fileName;
  Button loadFile;
  Button saveFile;
  Button quitButton;

  Button pgUp;
  Button pgDown;
  bool notYetLoaded;
  int page;

  ImageIcon currEditReel[3];
  ComboBox currEditType[3];
  LineEdit currEditBpVal[3];
  Label currEditBpLbl[3];
  size_t numEditReels;

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
    setGeometry({ 130, 130, 650, 500 });

    //Whee...
    MsgBox.create();

    //Ensure that the testReel can at least size itself (we can add images later).
    page = 0;
    numTestReels = 4;
    numEditReels = 3;
    notYetLoaded = true;
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
    for (size_t i=0; i<numEditReels; i++) {
    	currEditReel[i].setImage(rouletteSlots.find(0xFF)->second.img);
    	currEditBpLbl[i].setFont("Arial, 10, bold");
    	currEditBpLbl[i].setText("BP");
    	for (size_t j=0; j<comboLabels.size(); j++) {
    		currEditType[i].append(comboLabels[j]);
    	}
    }

    //Initialize callbacks
    for (size_t i=0; i<numTestReels; i++) {
    	testReels[i].setClickCallback(i, [this](size_t rowID, size_t slotID) {
    		if (rowID>=128 || notYetLoaded) {
    			return; //Again, must be a better way of doing this...
    		}

    		//Retrieve the tuple for this row:
    		std::tuple<SingleSlot, SingleSlot, SingleSlot>& reels = testReels[rowID].slots[slotID];
    		for (size_t r=0; r<3; r++) {
    			//Again, I mildly regret using tuples for array-like data.
    			SingleSlot& curr = (r==0)?std::get<0>(reels):(r==1)?std::get<1>(reels):std::get<2>(reels);

    			//Update the image, text, and combo box.
    			unsigned int id = (rouletteSlots.count(curr.statusID)>0) ? curr.statusID : 0xFF;
    			currEditReel[r].setImage(curr.icon.getImage());
    			currEditBpVal[r].setText(curr.numBP);
    			currEditType[r].setSelection(rouletteSlots.find(id)->second.comboID);
    		}
    	}
    	);
    }

    //Layout
    {
    //ScopedLayoutLock lock(&layout);
    for (size_t i=0; i<numTestReels; i++) {
    	//lock.append(&testReels[i].getLayout());
    }

    layout.setMargin(10);
    layout.append(loadFile, {{fileName, 10}}, {{0.0}});
    layout.append(fileName, {{0.0}, {fileName, 150, Anchor::Left}}, {Centered, {loadFile, 1}});
    layout.append(saveFile, {{loadFile, 10}}, {{loadFile, 0, Anchor::Top}});
    layout.append(quitButton, {{}, {1.0}}, {{loadFile, 0, Anchor::Top}});
    Layout* lastLayout = nullptr;
    for (size_t i=0; i<numTestReels; i++) {
    	AnchorLayout& reel = testReels[i].getLayout();
    	int suggWidth = testReels[i].getSuggestedMinimumSize().width;
    	int suggHeight = testReels[i].getSuggestedMinimumSize().height;
    	if (!lastLayout) {
    		layout.append(reel, {Centered, {0.5, -30}, suggWidth}, {{loadFile, 20}, {reel, suggHeight, Anchor::Top}});
    	} else {
    		layout.append(reel, {{*lastLayout, 0, Anchor::Left}, {reel, suggWidth, Anchor::Left}}, {{*lastLayout}, {reel, suggHeight, Anchor::Top}});
    	}
    	lastLayout = &reel;
    }
    layout.append(pgUp, {{testReels[0].getLayout()}}, {{testReels[0].getLayout(), 5, Anchor::Top}});
    layout.append(pgDown, {{testReels[numTestReels-1].getLayout()}}, {{}, {testReels[numTestReels-1].getLayout(), -5, Anchor::Bottom}});

    for (size_t i=0; i<numEditReels; i++) {
    	if (i==0) {
    		layout.append(currEditType[i], {{testReels[0].getLayout(), 10, Anchor::Left}}, {{currEditReel[i], 5}});
    	} else {
    		layout.append(currEditType[i], {{currEditType[i-1], 20}}, {{currEditType[i-1], 0, Anchor::Top}});
    	}
    	layout.append(currEditReel[i], {Centered, {currEditType[i]}}, {{testReels[numTestReels-1].getLayout(), 20}});
		layout.append(currEditBpLbl[i], {{}, {currEditType[i], 0, Anchor::Right}}, {{currEditType[i], 5}});
		layout.append(currEditBpVal[i], {{currEditType[i], 0, Anchor::Left}, {currEditBpLbl[i], -5}}, {Centered, {currEditBpLbl[i], 1}});
    }

    //Master layout
    {
    	ScopedLayoutLock lock(&layout);
    	append(layout);
    }


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
    		notYetLoaded = false;
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

    saveFile.onTick = [&MsgBox] {
    	MsgBox.setVisible(true);
    };

    onClose = quitButton.onTick = [&testReels, &layout, &numTestReels] {
    	//ScopedLayoutLock lock(&layout);
    	/*layout.setSkipGeomUpdates(true);
        for (size_t i=0; i<numTestReels; i++) {
        	testReels[i].getLayout().setSkipGeomUpdates(true);
        }*/
    	OS::quit();
    };
    } //End of layout lock

    std::cout <<" Set visible\n";
    setVisible();
    std::cout <<" Ready\n";
  }
} application;


int main(int argc, char* argv[])
{
  application.create();
  std::cout <<" Create done\n";
  OS::main();
  return 0;
}
