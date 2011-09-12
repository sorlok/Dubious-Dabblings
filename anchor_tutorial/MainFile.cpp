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

    AnchorPoint okLeft(0.0);
    AnchorPoint okRight = {};
    AnchorPoint quitLeft = {};
    AnchorPoint quitRight(1.0);
    AnchorPoint bothTop(0.0);
    AnchorPoint bothBottom = {};
    layout.append(quitButton, {Centered, {0.5}}, {Centered, {0.5}});

    Axis okHorizontal(okLeft, okRight);
    Axis quitHorizontal(quitLeft, quitRight);
    Axis bothVertical(bothTop, bothBottom);

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

