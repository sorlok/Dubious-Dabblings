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

  ListView configPanels;
  Label settingsCaption;
  Label rhs1;
  Label rhs2;
  Label rhs3;

  void create() {
    setTitle("BSNES Config Mockup");
    setGeometry({ 128, 128, 640, 480 });

    //Initialize components
    configPanels.append("Video");
    configPanels.append("Audio");
    configPanels.append("Input");
    configPanels.append("Advanced");
    configPanels.setSelection(3);
    settingsCaption.setText("Advanced Settings");
    settingsCaption.setFont(Font("Arial, 20, Bold Italic"));
    rhs1.setText("Option 1");
    rhs2.setText("Option 2");
    rhs3.setText("Option 3");

    //Perform layout
    layout.append(configPanels, {0.0, 5}, {0.0, 5}, {configPanels, 120}, {1.0, -5});
    layout.append(sublayout, {configPanels, 10}, {0.0}, {1.0}, {1.0});
    sublayout.append(settingsCaption, {0.0}, {0.0});
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
