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
    layout.append(okButton, {{0.0}}, {{0.0}});
    layout.append(quitButton, {{}, {1.0}}, {{0.0}});
    layout.append(fancyButton, {{okButton, 5, Anchor::Center},{quitButton, -5, Anchor::Center}}, {{0.0, 30}});

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

