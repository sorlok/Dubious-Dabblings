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

  //Set a margin for 1.0 and 0.0 percent values.
  layout.setMargin(5);

  //Horizontal layout for the Ok button
  AnchorPoint okLeft(0.0);
  AnchorPoint okRight(0.0, 150);

  //Horizontal layout for the Quit button
  AnchorPoint quitLeft(1.0, -150);
  AnchorPoint quitRight(1.0);

  //Vertical layout is the same for both buttons
  AnchorPoint bothTop(0.0);
  AnchorPoint bothBottom(0.0, 75);

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

