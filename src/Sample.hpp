#include <phoenix/phoenix.hpp>
using namespace nall;
using namespace phoenix;

struct Canvas2 : public phoenix::Canvas {
	Canvas2() {}

	//NOTE: The only change here is that Canvas::update() was made virtual.
	virtual void update() {
		uint32_t* buffer_ = buffer();
		const Geometry& geom = geometry();

		for (size_t i=0; i<geom.width*geom.height; i++) {
			buffer_[i] = 0xFF0000;
		}

		Canvas::update();
	}
};


struct Application : Window {
  VerticalLayout vertLayout;
  HorizontalLayout horizLayout;

  Label helloLabel;
  Button okButton;
  Button quitButton;
  Canvas2 bigCanvas;
  //Button bigButton;

  void create() {
    setTitle("Test Application");
    setGeometry({ 128, 128, 640, 480 });

    helloLabel.setText("Hello, world!");
    okButton.setText("Ok");
    quitButton.setText("Quit");
    //bigButton.setText("This button should take all remaining width");

    horizLayout.append(okButton, 100, 30, 5);
    horizLayout.append(quitButton, 100, 30, 5);

    vertLayout.setMargin(5);
    vertLayout.append(helloLabel, 0, 0, 5);
    vertLayout.append(horizLayout, 0, 0, 10);
    vertLayout.append(bigCanvas, ~0, ~0, 5); //NOTE: If we append bigButton here, it'll work fine.
    append(vertLayout);

    onClose = quitButton.onTick = &OS::quit;

    setVisible();
  }
} application;

int main() {
  application.create();
  OS::main();
  return 0;
}
