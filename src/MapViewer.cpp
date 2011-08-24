#include <iostream>

#include <phoenix/phoenix.hpp>
#include <nall/png.hpp>

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

  //Kinda messy, I know
  bool loadedMapOnce;
  Geometry scrollOffset;

  void create() {
    //Do window tasks
    setTitle("Test Application");
    setGeometry({ 128, 128, 640, 480 });

    helloLabel.setText("Map Viewer");
    okButton.setText("Load Map");
    quitButton.setText("Quit");

    layout.setMargin(5);                       //layout border in pixels
    layout.append(helloLabel, ~0, 0, 5);       //width, height, optional spacing
    controlLayout.append(okButton, ~0, 0, 5);  //width, height of  0 = use least amount of space possible to display all text
    controlLayout.append(quitButton, ~0, 0);   //width, height of ~0 = use all available width (divides when multiple controls use ~0)
    layout.append(controlLayout, 10);              //nested layouts inside of other layouts (can accept optional spacing parameter)
    layout.append(myCanvas, ~0, ~0, 5);
    append(layout);                            //both buttons will take 50% of available window client width

    //Get a reference to the canvas's buffer:
    PremultImage& bkgrd = myCanvas.getBufferedImage();
    unsigned int lastW = 0;  //For some reason, capturing as a geometry crashes.
    unsigned int lastH = 0;

    //Draw something simple onto the background
    loadedMapOnce = false;
    scrollOffset = {0,0,0,0};
    bkgrd.resetSize({0, 0, 900, 600});

    //Prepare arrows
    myCanvas.loadArrowMarkings("arrow.png");

    unsigned int w = bkgrd.getSize().width;
    unsigned int h = bkgrd.getSize().height;

    bkgrd.fillRect({0, 0, w/2, h/2}, {0xFF, 0x00, 0x00});
    bkgrd.fillRect({w/4, h/4, w/2, h/2}, {0x80, 0x00, 0x00, 0xFF});

    onClose = quitButton.onTick = &OS::quit;

    okButton.onTick = [this, &bkgrd, &scrollOffset]() {
    	if (!loadedMapOnce) {
    		GameMap::InitTMXMap(myMap, "map_test.tmx");
    		loadedMapOnce = true;
    		myMap.PaintImage(bkgrd);
    		myCanvas.paintArrows();
    		scrollOffset.x = 0;//myCanvas.bufferSize().width/2 - bkgrd.getSize().width/2;
    		scrollOffset.y = 0;//myCanvas.bufferSize().height/2 - bkgrd.getSize().height/2;
    		myCanvas.setImageOffset(scrollOffset);
    		myCanvas.update();
    	}
    };

    onSize = [this, &bkgrd, &scrollOffset, &lastW, &lastH]() {
    	//Has the buffer actually been destroyed?
    	if (myCanvas.geometry().width==lastW && myCanvas.geometry().height==lastH) {
    		return;
    	}

    	//Save new geometry
    	lastW = myCanvas.geometry().width;
    	lastH = myCanvas.geometry().height;

    	//Update offset
    	if (loadedMapOnce) {
    		if (bkgrd.getSize().width < myCanvas.geometry().width) {
    			scrollOffset.x = myCanvas.geometry().width/2 - bkgrd.getSize().width/2;
    		}
    		if (bkgrd.getSize().height < myCanvas.geometry().height) {
    			scrollOffset.y = myCanvas.geometry().height/2 - bkgrd.getSize().height/2;
    		}
    		myCanvas.setImageOffset(scrollOffset);
    	}

    	//TODO: Currently, "sizing" also includes dragging the window. This is pretty
    	//      bad for performance; dragging shouldn't destroy the buffer() pointer.
    	//      Will have to edit the Canvas class to fix.... I might not do this for a while.
    	myCanvas.update();
    };

    myCanvas.onMotion = [this, &bkgrd](unsigned int localX, unsigned int localY, MOVE_FLAG flag) {
    	if (flag==MOVE_FLAG::MOVE) {
    		if (loadedMapOnce) {
    			//If the mouse is near the edges of the screen, increase the offset
    			Geometry oldOffset = scrollOffset;
    			if (bkgrd.getSize().width >= myCanvas.geometry().width) {
    				if (localX < 32) {
    					scrollOffset.x += myMap.tileSize/4;
    				} else if (localX >= myCanvas.geometry().width - 32) {
    					scrollOffset.x -= myMap.tileSize/4;
    				}

    				//Bound
    				scrollOffset.x = nall::max(nall::min(0, scrollOffset.x), (int)myCanvas.geometry().width-(int)bkgrd.getSize().width);
    			}
    			if (bkgrd.getSize().height >= myCanvas.geometry().height) {
    				if (localY < 32) {
    					scrollOffset.y += myMap.tileSize/4;
    				} else if (localY >= myCanvas.geometry().height - 32) {
    					scrollOffset.y -= myMap.tileSize/4;
    				}

    				//Bound
    				scrollOffset.y = nall::max(nall::min(0, scrollOffset.y), (int)myCanvas.geometry().height-(int)bkgrd.getSize().height);
    			}

    			if (oldOffset.x!=scrollOffset.x || oldOffset.y!=scrollOffset.y) {
    				//Update
    				myCanvas.setImageOffset(scrollOffset);
    				myCanvas.update();
    			}
    		} else {
    			//Just track the mouse
    			int x = ((int)localX) - bkgrd.getSize().width/2;
    			int y = ((int)localY) - bkgrd.getSize().height/2;

    			myCanvas.setImageOffset(Geometry(x, y, 0, 0));
    			myCanvas.update();
    		}
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

