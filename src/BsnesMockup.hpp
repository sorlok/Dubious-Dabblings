#include <phoenix/phoenix.hpp>
#include "phoenix_ext/attach-layout.hpp"

//For brevity
using namespace nall;
using namespace phoenix;
typedef Attachment::ANCHOR ANCHOR;
typedef Attachment::SPECIAL SPECIAL;


struct Application : Window {
  AttachLayout layout;
  AttachLayout sublayout;

  ListView lhs;
  Label rhs1;
  Label rhs2;
  Label rhs3;

  void create() {
    setTitle("BSNES Config Mockup");
    setGeometry({ 128, 128, 640, 480 });

    lhs.append("Video");
    lhs.append("Audio");
    lhs.append("Input");
    lhs.append("Advanced");
    lhs.setSelection(3);
    rhs1.setText("Option 1");
    rhs2.setText("Option 2");
    rhs3.setText("Option 3");

    layout.append(lhs, {0.0, 5}, {0.0, 5}, {lhs, 120}, {1.0, -5});
    layout.append(sublayout, {lhs, 10}, {0.0}, {1.0}, {1.0});
    sublayout.append(rhs1, {0.0}, {0.0});
    sublayout.append(rhs2, {0.333}, {0.0});
    sublayout.append(rhs3, {0.667}, {0.0});
    append(layout);

    onClose = [&layout, &sublayout] {
    	layout.setSkipGeomUpdates(true);
    	sublayout.setSkipGeomUpdates(true);
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
