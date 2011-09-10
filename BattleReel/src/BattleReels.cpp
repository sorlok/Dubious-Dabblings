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
  VerticalLayout layoutVert;
  HorizontalLayout layoutHoriz;

  Label helloLabel;
  Button okButton;
  Button quitButton;

  ImageIcon testIcon1;
  ImageIcon testIcon2;
  ImageIcon testIcon3;
  ImageIcon testIcon4;
  ImageIcon testIcon5;
  ImageIcon testIcon6;
  ImageIcon testIcon7;

  SingleReel testReel;

  void create() {
    //Do window tasks
    setTitle("Test Application");
    setGeometry({ 130, 130, 700, 490 });

    //Ensure that the testReel can at least size itself (we can add images later).
    testReel.loadData(rouletteSlots);

    helloLabel.setText("Map Viewer");
    okButton.setText("Load Map");
    quitButton.setText("Quit");
    testIcon1.setImage(rouletteSlots.find(10)->second.img);
    testIcon2.setImage(rouletteSlots.find(10)->second.img);
    testIcon3.setImage(rouletteSlots.find(19)->second.img);
    testIcon4.setImage(rouletteSlots.find(18)->second.img);
    testIcon5.setImage(rouletteSlots.find(10)->second.img);
    testIcon6.setImage(rouletteSlots.find(15)->second.img);
    testIcon7.setImage(rouletteSlots.find(1)->second.img);

    //Horizontal layout
    layoutHoriz.append(okButton, 100, 30, 5);
    layoutHoriz.append(quitButton, 100, 30, 5);
    /*layoutHoriz.append(testIcon2, 0, 0, 5);
    layoutHoriz.append(testIcon3, 0, 0, 5);
    layoutHoriz.append(testIcon4, 0, 0, 5);
    layoutHoriz.append(testIcon5, 0, 0, 5);
    layoutHoriz.append(testIcon6, 0, 0, 5);
    layoutHoriz.append(testIcon7, 0, 0, 5);*/

    //Vertical layout
    layoutVert.setMargin(5);
    layoutVert.append(helloLabel, 0, 0, 5);
    layoutVert.append(layoutHoriz, 0, 0, 10);
    layoutVert.append(testReel.getLayout(), testReel.getSuggestedMinimumSize().width, testReel.getSuggestedMinimumSize().height, 5);

    //Master layout
    append(layoutVert);


    onClose = quitButton.onTick = [&testReel] {
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



