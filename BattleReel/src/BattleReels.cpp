#include <phoenix/phoenix.hpp>
#include <iostream>
#include <string>
#include <map>

#include "phoenix_ext/anchor-layout.hpp"
#include "phoenix_ext/image-icon.hpp"
#include "components/SingleReel.hpp"

using std::cout;
using std::endl;
using namespace nall;
using namespace phoenix;
typedef AnchorPoint::Anchor Anchor;


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

  Button loadFile;
  Button saveFile;
  Button quitButton;

  SingleReel testReel;

  void create() {
    //Do window tasks
    setTitle("FF7 Battle Reel Editor");
    setGeometry({ 130, 130, 700, 490 });

    //Ensure that the testReel can at least size itself (we can add images later).
    testReel.loadData(rouletteSlots);
    loadFile.setText("Load File");
    saveFile.setText("Save Changes");
    quitButton.setText("Quit");

    //SingleReels are a bit strange.
    Geometry reelSize = testReel.getSuggestedMinimumSize();

    //Layout
    layout.setMargin(10);
    layout.append(loadFile, {{0.0}}, {{0.0}});
    layout.append(saveFile, {{loadFile, 10}}, {{loadFile, 0, Anchor::Top}});
    layout.append(quitButton, {{1.0}}, {{loadFile, 0, Anchor::Top}});
    layout.append(testReel.getLayout(), {{0.0}, {testReel.getLayout(), reelSize.width, Anchor::Left}}, {{0.5}, {testReel.getLayout(), reelSize.height, Anchor::Top}});

    //Master layout
    append(layout);


    onClose = quitButton.onTick = [&testReel, &layout] {
    	layout.setSkipGeomUpdates(true);
    	testReel.getLayout().setSkipGeomUpdates(true);
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



