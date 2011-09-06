#include <phoenix/phoenix.hpp>
#include "phoenix_ext/attach-layout.hpp"

#include <iostream>

using namespace nall;
using namespace phoenix;

struct Application : Window {
  AttachLayout layout;

  Label helloLabel;
  Button okButton;
  Button quitButton;
  Button rightButton;
  Button bigButton;

  void create() {
    setTitle("Test Application");
    setGeometry({ 128, 128, 640, 480 });

    helloLabel.setText("Hello, world!");
    okButton.setText("Ok");
    quitButton.setText("Quit");
    rightButton.setText("Rightish");
    bigButton.setText("This button should take all remaining width");

    /*std::cout <<"Hello lbl: " <<&helloLabel <<"\n";
    std::cout <<"Ok button: " <<&okButton <<"\n";
    std::cout <<"Quit button: " <<&quitButton <<"\n";
    std::cout <<"Right button: " <<&rightButton <<"\n";
    std::cout <<"Big button: " <<&bigButton <<"\n";*/

    layout.append(helloLabel, {0.0}, {0.0}, {0.0, 150}, {0.1});
    layout.append(okButton, {0.0}, {0.1}, {0.0, 150}, {0.2});
    layout.append(quitButton, {0.0, 160}, {0.1}, {0.0, 360}, {0.2});
    layout.append(rightButton, {1.0, -150}, {0.1}, {1.0}, {0.2});
    layout.append(bigButton, {0.0}, {0.2}, {1.0}, {1.0});
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
