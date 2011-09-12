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
  Button fancyButton;
  Button quitButton;
  Label normalLbl;

  void create() {
    setTitle("Anchor Layout Tutorial");
    setGeometry({ 128, 128, 640, 480 });

    okButton.setText("Ok"); 
    fancyButton.setText("Fancy");
    quitButton.setText("Quit");
    normalLbl.setText("A Very Wide Label With Lots Of Text");

    layout.append(normalLbl, {{0.3}}, {{0.0, 5}});
    layout.append(okButton, {Centered, {normalLbl, 0, Anchor::Right}}, {{normalLbl, 5}});
    layout.append(fancyButton, {Centered, {okButton}, 150}, {{okButton, 5}, {fancyButton, 75}});
    layout.append(quitButton, {Centered, {okButton}}, {{fancyButton, 5}});

    okButton.onTick = [this] {
      layout.append(okButton, {Centered, {0.5}, 150}, {Centered, {0.2}, 75});
    };


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

