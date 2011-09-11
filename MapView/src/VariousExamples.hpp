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

  Button btn1;
  Button btn2;
  Button btn3;

  void create() {
    setTitle("Test Application");
    setGeometry({ 128, 128, 500, 150 });

    btn1.setText("Open");
    btn2.setText("Save");
    btn3.setText("Quit");

    layout.setMargin(5);
    layout.append(btn1, {{0.0}}, {{0.0}});
    layout.append(btn2, {{btn1, 0, Anchor::Center}}, {{btn1, 5}});
    layout.append(btn3, {{btn2, 0, Anchor::Center}}, {{btn2, 5}});
    append(layout);

    onClose = [&layout] {
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
