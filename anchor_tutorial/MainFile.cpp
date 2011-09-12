#include <phoenix/phoenix.hpp>
#include "anchor-layout.hpp"

using namespace nall;
using namespace phoenix;

//For brevity
typedef AnchorPoint::Anchor Anchor;
AnchorPoint Centered = Axis::Centered();


struct Application : Window {
  AnchorLayout layout;
  Button okButton;
  Button quitButton;

  void create() {
    setTitle("Anchor Layout Tutorial");
    setGeometry({ 128, 128, 640, 480 });

    okButton.setText("Ok"); 
    quitButton.setText("Quit");

  layout.setMargin(5);
  Axis vert = {{0.0},{0.0,75}};
  layout.append(okButton, {{0.0},{0.0,150}}, vert);
  layout.append(quitButton, {{1.0, -150},{1.0}}, vert);

    append(layout);

    onClose = quitButton.onTick = [] {
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

