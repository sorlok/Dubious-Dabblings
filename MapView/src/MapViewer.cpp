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


//Test class for use with Parrot
class TestBase {
public:
	virtual int getValue() {
		return 0;
	}
};


struct Application : Window {
  struct MsgBox : Window {
	AnchorLayout layout;
	Label lbl;
	Button ok;

	void create() {
		setTitle("");
		setGeometry({200, 200, 250, 75});

		lbl.setText("");
		ok.setText("Ok");

		layout.setMargin(10);
		layout.append(lbl, {Centered, {0.5}}, {Centered, {0.5, -10}});
		layout.append(ok, {Centered, {0.5}}, {{lbl, 5}});

		ok.onTick = [this] {
			setVisible(false);
		};

		append(layout);
	}

	void show(const nall::string& title, const nall::string& text) {
		setTitle(title);
		lbl.setText(text);
		setVisible(true);
	}
  } MsgBox;

  AnchorLayout layout;

  Label helloLabel;
  Button loadMapBtn;
  Button testParrotBtn;
  Button quitButton;
  GameMap myMap;

  CanvasExt myCanvas;
  HorizontalScrollBar hScroll;
  VerticalScrollBar vScroll;

  //Kinda messy, I know
  bool loadedMapOnce;
  Geometry scrollOffset;

  void recalcScrollBars() {
	  PremultImage& bkgrd = myCanvas.getBufferedImage();
	  int widthDiff = static_cast<int>(bkgrd.getSize().width) - myCanvas.geometry().width;
	  int heightDiff = static_cast<int>(bkgrd.getSize().height) - myCanvas.geometry().height;

	  //Enabled
	  hScroll.setEnabled(widthDiff>0);
	  vScroll.setEnabled(heightDiff>0);

	  //Length
	  hScroll.setLength(widthDiff>0?widthDiff:100);
	  vScroll.setLength(heightDiff>0?heightDiff:100);

	  //Position
	  hScroll.setPosition(0);
	  vScroll.setPosition(0);
  }


  void updateScrollbar(bool isHoriz) {
	  //NOTE: This won't work quite right because hScroll or vScroll might be 0 (if the component is too small).
	  myCanvas.setImageOffset({-hScroll.position(), -vScroll.position(), 0, 0});
	  myCanvas.update();
  }

  void parrotTest() {
	  TestBase t;
	  int value = t.getValue();
	  MsgBox.show("Test", {"Base(0), Override(1) returned: " , value});
  }


  void initComponents() {
    ScopedLayoutLock lock(&layout);
    helloLabel.setText("Map Viewer");
    loadMapBtn.setText("Load Map");
    testParrotBtn.setText("Test Parrot VM");
    quitButton.setText("Quit");
    hScroll.setEnabled(false);
    vScroll.setEnabled(false);

    //Button layouts
    layout.setMargin(5);
    layout.append(helloLabel, {{0.0}}, {0.0});
    layout.append(loadMapBtn, {{0.0}, {loadMapBtn, 100, Anchor::Left}}, {{helloLabel, 10}, {loadMapBtn, 30, Anchor::Top}});
    layout.append(testParrotBtn, {{loadMapBtn, 5}, {testParrotBtn, 100, Anchor::Left}}, {{loadMapBtn, 0, Anchor::Top}, {testParrotBtn, 30, Anchor::Top}});
    layout.append(quitButton, {{testParrotBtn, 5}, {quitButton, 100, Anchor::Left}}, {{loadMapBtn, 0, Anchor::Top}, {quitButton, 30, Anchor::Top}});

    //Canvas and scroll bars
    layout.append(myCanvas, {{0.0}, {vScroll, -5}}, {{loadMapBtn, 5}, {hScroll, -5}});
    layout.append(hScroll, {{myCanvas, 0, Anchor::Left}, {myCanvas, 0, Anchor::Right}}, {{}, {1.0}});
    layout.append(vScroll, {{}, {1.0}}, {{myCanvas, 0, Anchor::Top}, {myCanvas, 0, Anchor::Bottom}});

    //Done
    append(layout);

    //Get a reference to the canvas's buffer:
    PremultImage& bkgrd = myCanvas.getBufferedImage();

    //Draw something simple onto the background
    loadedMapOnce = false;
    scrollOffset = {0,0,0,0};
    bkgrd.resetSize({0, 0, 900, 600});

    unsigned int w = bkgrd.getSize().width;
    unsigned int h = bkgrd.getSize().height;

    bkgrd.fillRect({0, 0, w/2, h/2}, {0xFF, 0x00, 0x00});
    bkgrd.fillRect({w/4, h/4, w/2, h/2}, {0x80, 0x00, 0x00, 0xFF});

    onClose = quitButton.onTick = [&layout] {
      layout.setSkipGeomUpdates(true);
   	  OS::quit();
    };

    hScroll.onChange = [this]() {
    	updateScrollbar(true);
    };
    vScroll.onChange = [this]() {
    	updateScrollbar(false);
    };

    loadMapBtn.onTick = [this]() {
    	if (!loadedMapOnce) {
    		PremultImage& bkgrd = myCanvas.getBufferedImage();
    		GameMap::InitTMXMap(myMap, "map_test.tmx");
    		loadedMapOnce = true;
    		myMap.PaintImage(bkgrd);
    		scrollOffset.x = 0;//myCanvas.bufferSize().width/2 - bkgrd.getSize().width/2;
    		scrollOffset.y = 0;//myCanvas.bufferSize().height/2 - bkgrd.getSize().height/2;

    		//Scroll bars
    		recalcScrollBars();

    		myCanvas.setImageOffset(scrollOffset);
    		myCanvas.update();
    	}
    };

    testParrotBtn.onTick = { &Application::parrotTest, this };

    onSize = [this]() {
    	//Has the buffer actually been destroyed?
    	if (!myCanvas.needsResize()) {
    		return;
    	}

    	//Update offset
    	if (loadedMapOnce) {
    		PremultImage& bkgrd = myCanvas.getBufferedImage();
    		recalcScrollBars();

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

    myCanvas.onMotion = [this](unsigned int localX, unsigned int localY, MOVE_FLAG flag) {
    	if (flag==MOVE_FLAG::MOVE) {
    		if (!loadedMapOnce) {
    			//Just track the mouse; simple feedback.
    			PremultImage& bkgrd = myCanvas.getBufferedImage();
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
    //Create our message box window
    MsgBox.create();

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
