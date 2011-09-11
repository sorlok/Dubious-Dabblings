#include <phoenix/phoenix.hpp>
#include "phoenix_ext/attach-layout.hpp"

#include <vector>
#include <string>
#include <iostream>

using namespace nall;
using namespace phoenix;
using std::vector;

//For brevity
typedef AnchorPoint::Anchor Anchor;
AnchorPoint Centered = Axis::Centered();

const vector<vector<nall::string>> layers = {
	{"Restore", "Damage"},
	{"Cure", "Life", "Fire", "Lightning"},
	{"Cure All", "Full Cure", "Full Life", "", "Inferno", "", "Electrocute", "Zap"},
};

struct Application : Window {
  //To test nesting byuu's layouts...
  HorizontalLayout lowerButtonLayout;
  Button okBtn;
  Button cancelBtn;

  AnchorLayout layout;
  Label techStartLabel;
  Button layerButtons[20]; //Hackish... Button doesn't want to be put into a vector


  void create() {
    setTitle("Tech Tree Test Layout");
    setGeometry({ 128, 128, 700, 500 });

    //Ok/Cancel
    techStartLabel.setText("Tree of Techniques");
    okBtn.setText("Ok");
    cancelBtn.setText("Cancel");

    //Make all our buttons at once to ensure our references don't get invalidated.
    size_t id = 0;
    for (auto row : layers) {
    	for (auto col : row) {
    		layerButtons[id++].setText(col);
    	}
    }

    //First, the lower buttons
    lowerButtonLayout.setMargin(10);
    lowerButtonLayout.append(okBtn, 200, 75);
    lowerButtonLayout.append(cancelBtn, 150, 50, 0.5);

    //Append our title and lower button layouts since they're the easiest
    layout.append(techStartLabel, {Centered, {0.5}}, {{0.0, 20}});
    layout.append(lowerButtonLayout, {Centered, {0.5}}, {{},{1.0, -20}});

    //And now the buttons! (We skip blank buttons)
    id=0;
    Sizable* lastRow = &techStartLabel;
    for (size_t row=0; row<layers.size(); row++) {
    	double startOffset = 1.0 / (layers[row].size()*2.0);
    	double offsetIncrement = startOffset * 2.0;
    	Sizable* lastItem = nullptr;
    	for (size_t col=0; col<layers[row].size(); col++) {
    		//Skip it?
    		Button& b = layerButtons[id++];
    		if (layers[row][col].length()>0) {
    			layout.append(b, {Centered, {startOffset}}, {{*lastRow, 10}});
    		}

    		//Increment
    		startOffset += offsetIncrement;
    		lastItem = &b;
    	}
    	//Increment
    	lastRow = lastItem;
    }

    //Done
    append(layout);

    onClose = [&layout] {
    	layout.setSkipGeomUpdates(true);
    	OS::quit();
    };

    setVisible();
  }
} application;

int main() {
  application.create();
  OS::main();
  return 0;
}
