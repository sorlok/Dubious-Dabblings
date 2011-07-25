#include <iostream>

#include <phoenix/phoenix.hpp>

#include "phoenix_ext/CanvasExt.hpp"
#include "gamedata/GameMap.hpp"

using namespace nall;
using namespace phoenix;



struct Application : Window {
  VerticalLayout layout;
  Label helloLabel;
  HorizontalLayout controlLayout;
  Button okButton;
  Button quitButton;
  CanvasExt myCanvas;
  GameMap myMap;

  void create() {
    //Do window tasks
    setTitle("Test Application");
    setGeometry({ 128, 128, 640, 480 });

    helloLabel.setText("Hello, world!");
    okButton.setText("Load Map");
    quitButton.setText("Quit");

    layout.setMargin(5);                       //layout border in pixels
    layout.append(helloLabel, ~0, 0, 5);       //width, height, optional spacing
    controlLayout.append(okButton, ~0, 0, 5);  //width, height of  0 = use least amount of space possible to display all text
    controlLayout.append(quitButton, ~0, 0);   //width, height of ~0 = use all available width (divides when multiple controls use ~0)
    layout.append(controlLayout);              //nested layouts inside of other layouts (can accept optional spacing parameter)
    layout.append(myCanvas, ~0, ~0, 5);
    append(layout);                            //both buttons will take 50% of available window client width

    //Get a reference to the canvas's buffer:
    PremultImage& bkgrd = myCanvas.getBufferedImage();

    //Draw something simple onto the background
    bkgrd.resetSize({0, 0, 900, 600});

    unsigned int w = bkgrd.getSize().width;
    unsigned int h = bkgrd.getSize().height;

    bkgrd.fillRect({0, 0, w/2, h/2}, {0xFF, 0x00, 0x00});
    bkgrd.fillRect({w/4, h/4, w/2, h/2}, {0x80, 0x00, 0x00, 0xFF});

    onClose = quitButton.onTick = &OS::quit;

    okButton.onTick = [this, &bkgrd]() {
    	GameMap::InitTMXMap(myMap, "map_test.tmx");
    	myMap.PaintImage(bkgrd);
    	myCanvas.update();
    };


    onSize = [this]() {
    	//TODO: Currently, "sizing" also includes dragging the window. This is pretty
    	//      bad for performance; dragging shouldn't destroy the buffer() pointer.
    	//      Will have to edit the Canvas class to fix.... I might not do this for a while.
    	myCanvas.update();
    };

    myCanvas.onMotion = [this, &bkgrd](unsigned int localX, unsigned int localY, MOVE_FLAG flag) {
    	if (flag==MOVE_FLAG::MOVE) {
    		int x = ((int)localX) - bkgrd.getSize().width/2;
    		int y = ((int)localY) - bkgrd.getSize().height/2;

    		myCanvas.setImageOffset(Geometry(x, y, 0, 0));
    		myCanvas.update();
    	} else if (flag==MOVE_FLAG::LEAVE) {
    		//TODO: We can remove any "highlights" or whatnot here.
    		myCanvas.update();
    	}
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

