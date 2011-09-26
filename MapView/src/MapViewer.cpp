//Turn on debugging info  --note: This won't actually matter later when
//          using the library, since all output code is in the CPP file.
#define ANCHOR_LAYOUT_ERRORS_ON

//Minimal test for Phoenix layout error
//#include "Sample.hpp"

//Tests for new AttachLayout
//#include "LayoutTest.hpp"

//Various examples
//#include "VariousExamples.hpp"

//Test "Tech Tree" mockup
//#include "TechTree.hpp"

//Tests for the BSNES config menu via AttachLayout
//#include "BsnesMockup.hpp"

//Main code (comment out if using one of the header-style tests, above.
#include <iostream>

#include <phoenix/phoenix.hpp>
#include <nall/png.hpp>

#include "phoenix_ext/anchor-layout.hpp"
#include "phoenix_ext/CanvasExt.hpp"
#include "gamedata/GameMap.hpp"

using namespace nall;
using namespace phoenix;
typedef AnchorPoint::Anchor Anchor;
const AnchorPoint Centered = Axis::Centered();


struct Application : Window {
  AnchorLayout layout;

  Label helloLabel;
  Button okButton;
  Button quitButton;
  CanvasExt myCanvas;
  GameMap myMap;

  //Kinda messy, I know
  bool loadedMapOnce;
  Geometry scrollOffset;

  void initComponents() {
    ScopedLayoutLock lock(&layout);
    helloLabel.setText("Map Viewer");
    okButton.setText("Load Map");
    quitButton.setText("Quit");

    //Horizontal layout
    layout.setMargin(5);
    layout.append(helloLabel, {{0.0}}, {0.0});
    layout.append(okButton, {{0.0}, {okButton, 100, Anchor::Left}}, {{helloLabel, 10}, {okButton, 30, Anchor::Top}});
    layout.append(quitButton, {{okButton, 5}, {quitButton, 100, Anchor::Left}}, {{okButton, 0, Anchor::Top}, {quitButton, 30, Anchor::Top}});
    layout.append(myCanvas, {{0.0}, {1.0}}, {{okButton, 5}, {1.0}});

    //Done
    append(layout);

    //Get a reference to the canvas's buffer:
    PremultImage& bkgrd = myCanvas.getBufferedImage();

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

    onClose = quitButton.onTick = [&layout] {
      layout.setSkipGeomUpdates(true);
   	  OS::quit();
    };

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

    onSize = [this, &bkgrd, &scrollOffset]() {
    	//Has the buffer actually been destroyed?
    	if (!myCanvas.needsResize()) {
    		return;
    	}

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
  }

  void create() {
    //Do window tasks
    setTitle("Test Application");
    setGeometry({ 130, 130, 650, 490 });
    initComponents();
    setVisible();
  }
} application;

int main(int argc, char* argv[])
{
  application.create();
  OS::main();
  return 0;
}
