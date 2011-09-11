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
  Button btn4;
  Button btn5;

  void create() {
    setTitle("Test Application");
    setGeometry({ 128, 128, 300, 300 });

    btn1.setText("Big!");
    btn2.setText("North");
    btn3.setText("South");
    btn4.setText("West");
    btn5.setText("East");

    layout.setMargin(5);
    layout.append(btn5, {{btn3, 5}}, {Centered, {0.5}});
    layout.append(btn2, {Centered, {0.5}}, {{}, {btn5, -5}});
    layout.append(btn4, {{}, {btn2, -5}}, {Centered, {0.5}});
    layout.append(btn3, {Centered, {0.5}}, {{btn4, 5}});
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
