#include <iostream>

#include <phoenix/phoenix.hpp>

#include "phoenix_ext/CanvasExt.hpp"

using namespace nall;
using namespace phoenix;



struct Application : Window {
  VerticalLayout layout;
  Label helloLabel;
  HorizontalLayout controlLayout;
  Button okButton;
  Button quitButton;
  CanvasExt myCanvas;

  void create() {
    setTitle("Test Application");
    setGeometry({ 128, 128, 640, 480 });

    helloLabel.setText("Hello, world!");
    okButton.setText("Paint Canvas");
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
    //NOTE: This is big enough that it's slow to draw. Try implementing memcpy for speedup.
    bkgrd.resetSize({0, 0, 900, 600});

    unsigned int w = bkgrd.getSize().width;
    unsigned int h = bkgrd.getSize().height;

    bkgrd.fillRect({0, 0, w/2, h/2}, {0xFF, 0x00, 0x00});
    bkgrd.fillRect({w/4, h/4, w/2, h/2}, {0x80, 0x00, 0x00, 0xFF});
    //myCanvas.update();  //TODO: How to call once at startup?

    onClose = quitButton.onTick = &OS::quit;

    okButton.onTick = [this]() {
    	//myCanvas.update();
    };

    onSize = [this]() {
    	myCanvas.update();
    };

    myCanvas.onMotion = [this, &bkgrd](unsigned int localX, unsigned int localY, MOVE_FLAG flag) {
    	if (flag==MOVE_FLAG::MOVE) {
    		int x = ((int)localX) - bkgrd.getSize().width/2;
    		int y = ((int)localY) - bkgrd.getSize().height/2;

    		myCanvas.setImageOffset(Geometry(x, y, 0, 0));
    		myCanvas.update();
    	} else if (flag==MOVE_FLAG::LEAVE) {
    		myCanvas.update(); //Clean up painting artifacts
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

