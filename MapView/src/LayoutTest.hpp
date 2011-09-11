#include <phoenix/phoenix.hpp>
#include "phoenix_ext/attach-layout.hpp"

#include <iostream>

using namespace nall;
using namespace phoenix;

//For brevity
typedef AnchorPoint::Anchor Anchor;
AnchorPoint Centered = Axis::Centered();


struct Application : Window {
  AnchorLayout layout;

  Label helloLabel;
  LineEdit statusBar;
  Button okButton;
  Button quitButton;
  Button rightButton;
  Button rightButtonBack;
  Button bigButton;
  Button north;
  Button south;
  Button east;
  Button west;

  void create() {
    setTitle("Test Application");
    setGeometry({ 128, 128, 640, 480 });

    helloLabel.setText("Hello, world!");
    okButton.setText("Ok");
    quitButton.setText("Quit");
    rightButton.setText("Rightish");
    rightButtonBack.setText("Left of Rightish");
    bigButton.setText("Fully centered, 300x");
    statusBar.setText("(Status bar)");
    statusBar.setEditable(false);
    north.setText("North");
    south.setText("South");
    east.setText("East");
    west.setText("West");

    /*std::cout <<"Hello lbl: " <<&helloLabel <<"\n";
    std::cout <<"Ok button: " <<&okButton <<"\n";
    std::cout <<"Quit button: " <<&quitButton <<"\n";
    std::cout <<"Right button: " <<&rightButton <<"\n";
    std::cout <<"Big button: " <<&bigButton <<"\n";*/

    //HelloLabel is in the top-left corner.
    layout.append(helloLabel, {{0.0}}, {{0.0}});

    //OkButton is 10 pixelsunder HelloLabel
    layout.append(okButton, {{0.0}}, {{helloLabel, 10}});

    //QuitButton is right of OkButton. We force it to be 150 pixels long
    layout.append(quitButton, {{okButton, 10},{quitButton, 150, Anchor::Left}}, {{okButton, 0, Anchor::Top}});

    //Right button is at the far right side of the screen
    layout.append(rightButton, {{}, {1.0}}, {{okButton, 0, Anchor::Top}});

    //The next button just attaches to the left of this (we're testing right-attachments here)
    layout.append(rightButtonBack, {{}, {rightButton, -10}}, {{okButton, 0, Anchor::Top}});

    //Put a status bar at the bottom of the screen.
    layout.append(statusBar, {{0.0, 5},{1.0, -5}}, {{}, {1.0, -5}});

    //The final component simply takes up all remaining vertical space, and is centered horizontally
    layout.append(bigButton, {Centered, {0.5, 300}}, {Centered, {0.5, 300}});

    //These are fun!
    layout.append(north, {Centered, {bigButton, 0, Anchor::Center}}, {{}, {bigButton, -5}});
    layout.append(south, {Centered, {bigButton, 0, Anchor::Center}}, {{bigButton, 5}});
    layout.append(west, {{}, {bigButton, -5}}, {Centered, {bigButton, 0, Anchor::Center}});
    layout.append(east, {{bigButton, 5}}, {Centered, {bigButton, 0, Anchor::Center}});

    //Hook it all up
    append(layout);

    onClose = quitButton.onTick = [&layout] {
    	layout.setSkipGeomUpdates(true); //Example of skipping pointless update calculations.
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
