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

  AnchorPoint bothTop(0.0);
  AnchorPoint bothBottom = {};
  Axis bothVertical(bothTop, bothBottom);

  AnchorPoint okCenter({0.333});
  Axis okHorizontal(Centered, okCenter);

  AnchorPoint quitCentered({0.667});
  Axis quitHorizontal(Centered, quitCentered, 150);

  layout.setMargin(5);
  layout.append(okButton, okHorizontal, bothVertical);
  layout.append(quitButton, quitHorizontal, bothVertical);

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

