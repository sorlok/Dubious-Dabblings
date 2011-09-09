#include <phoenix/phoenix.hpp>
#include <iostream>

#include "phoenix_ext/anchor-layout.hpp"
#include "phoenix_ext/image-icon.hpp"

using namespace nall;
using namespace phoenix;


struct Application : Window {
  VerticalLayout layoutVert;
  HorizontalLayout layoutHoriz;

  Label helloLabel;
  Button okButton;
  Button quitButton;

  void create() {
    //Do window tasks
    setTitle("Test Application");
    setGeometry({ 130, 130, 650, 490 });

    helloLabel.setText("Map Viewer");
    okButton.setText("Load Map");
    quitButton.setText("Quit");

    //Horizontal layout
    layoutHoriz.append(okButton, 100, 30, 5);
    layoutHoriz.append(quitButton, 100, 30, 5);

    //Vertical layout
    layoutVert.setMargin(5);
    layoutVert.append(helloLabel, 0, 0, 5);
    layoutVert.append(layoutHoriz, 0, 0, 10);

    //Master layout
    append(layoutVert);


    onClose = quitButton.onTick = [&layoutHoriz, &layoutVert] {
        //layout.skipUpdate = true;
    	OS::quit();
    };


    setVisible();
  }
} application;


int main(int argc, char* argv[])
{
  application.create();
  OS::main();
  return 0;
}



