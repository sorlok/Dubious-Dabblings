#include <phoenix/phoenix.hpp>
#include <iostream>
#include <string>
#include <map>

#include "phoenix_ext/anchor-layout.hpp"
#include "phoenix_ext/image-icon.hpp"

using std::cout;
using std::endl;
using namespace nall;
using namespace phoenix;

//Attempt to load an image.
bool LoadPng(const nall::string& filename, nall::png& res) {
	if (res.decode(filename)) {
		//Not sure if transform() is required once to get data to point to the right thing.
		res.transform();
		return true;
	}
	return false;
}

const nall::string prefix = "slots/";
const nall::string suffix = ".png";
struct Slot {
	nall::string name;
	nall::png img;
	Slot(const nall::string& name) : name(name), img() {
		init(name);
	}
	Slot(const Slot& copy) : name(copy.name), img() {
		//It's expensive to reload the PNG each time, but phoenix::png doesn't declare a move constructor,
		//   so pointers will be deleted twice otherwise.
		init(name);
	}

private:
	void init(const nall::string& name) {
		if (!LoadPng({prefix, name, suffix}, img)) {
			cout <<"Can't find image: " <<name <<endl;
		}
		cout <<"Loaded image: " <<name <<endl;
	}
};

const std::map<unsigned int, Slot> rouletteSlots = {
	{0, {"accessory"}},
	{1, {"all"}},
	{2, {"armor"}},
	{3, {"command"}},
	{4, {"cure"}},
	{5, {"frog"}},
	{6, {"halfhp"}},
	{7, {"halfhpmp"}},
	{8, {"halfmp"}},
	{9, {"halfspeed"}},
	{10, {"independent"}},
	{11, {"item"}},
	{12, {"level10"}},
	{13, {"level5"}},
	{14, {"lucky"}},
	{15, {"magic"}},
	{16, {"mini"}},
	{17, {"poison"}},
	{18, {"summon"}},
	{19, {"support"}},
	{20, {"time"}},
	{21, {"weapon"}},
	{22, {"zeromp"}},
	{0xFF, {"unknown"}}
};



struct Application : Window {
  VerticalLayout layoutVert;
  HorizontalLayout layoutHoriz;

  Label helloLabel;
  Button okButton;
  Button quitButton;

  ImageIcon testIcon;

  void create() {
    //Do window tasks
    setTitle("Test Application");
    setGeometry({ 130, 130, 650, 490 });

    helloLabel.setText("Map Viewer");
    okButton.setText("Load Map");
    quitButton.setText("Quit");
    testIcon.setImage(rouletteSlots.find(1)->second.img);

    //Horizontal layout
    layoutHoriz.append(okButton, 100, 30, 5);
    layoutHoriz.append(quitButton, 100, 30, 5);
    layoutHoriz.append(testIcon, 0, 0, 5);

    //Vertical layout
    layoutVert.setMargin(5);
    layoutVert.append(helloLabel, 0, 0, 5);
    layoutVert.append(layoutHoriz, 0, 0, 10);

    //Master layout
    append(layoutVert);


    onClose = quitButton.onTick = [&layoutHoriz, &layoutVert] {
        //layout.skipUpdate = true;
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



