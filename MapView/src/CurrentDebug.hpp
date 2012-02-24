#include <phoenix/phoenix.hpp>
#include <nall/string.hpp>

#include <iostream>

using namespace nall;
using namespace phoenix;

struct Application : Window {
	HorizontalLayout layout;
	Button bProcess;
	ListView lvOutput;
	LineEdit baseN, baseR;

  void create() {
    setTitle("Test Application");
    setGeometry(Geometry(128, 128, 640, 480 ));

    layout.append(bProcess);
    layout.append(lvOutput);
    layout.append(baseN);
    layout.append(baseR);

    unsigned bn = integer(baseN.text()), br = integer(baseR.text());
    if (br>=0 && br<=bn) {
       lstring hdr;
       for (unsigned i = 0; i < br; ++i) {
     	  hdr.append((string){i>0?",":"","B",i+1});
       }
       lvOutput.setHeaderText(hdr);
    }

    bProcess.setText("Draw!");
    bProcess.onActivate = [&]() {
       lvOutput.reset();

    };

    //Hook it all up
    append(layout);

    onClose = [&layout] {
    //	layout.setSkipGeomUpdates(true); //Example of skipping pointless update calculations.
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
