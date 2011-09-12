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

  void create() {
    setTitle("Anchor Layout Tutorial");
    setGeometry({ 128, 128, 640, 480 });

    okButton.setText("Ok"); 
    fancyButton.setText("Fancy");
    quitButton.setText("Quit");


  layout.setMargin(5);
  layout.append(okButton, {{0.0}, {okButton, 150}}, {{0.0}, {okButton, 75,}});
  layout.append(quitButton, {{quitButton, -150}, {1.0}}, {{okButton, 0, Anchor::Top}, {quitButton, 75}});

    okButton.onTick = [this] {
//      layout.setSkipGeomUpdates(true);
//      layout.remove(okButton);
//      layout.setSkipGeomUpdates(false);
      layout.append(okButton, {Centered, {0.5}, 150}, {Centered, {0.2}, 75});
//      layout.append(quitButton, {{quitButton, -150}, {1.0}}, {{okButton, 0, Anchor::Top}, {quitButton, 75}});
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

