#include <phoenix/phoenix.hpp>
#include <nall/directory.hpp>
#include <iostream>
#include <string>
#include <map>

#include "phoenix_ext/anchor-layout.hpp"
#include "phoenix_ext/thumbnail-layout.hpp"
#include "phoenix_ext/image-icon.hpp"

#include "components/SingleReel.hpp"

using std::cout;
using std::endl;
using namespace nall;
using namespace phoenix;




struct Application : Window {
  struct MsgBox : Window {
	AnchorLayout layout;
	Label lbl;
	Button ok;

	void create() {
		AnchorPoint Centered = Axis::Centered();
		setTitle("Imagine!");
		setGeometry({200, 200, 180, 75});

		lbl.setText("Pretend a file opened.");
		ok.setText("Ok");

		layout.setMargin(5);
		layout.append(lbl, {Centered, {0.5}}, {Centered, {0.5, -10}});
		layout.append(ok, {Centered, {0.5}}, {{lbl, 5}});

		ok.onTick = [this] {
			setVisible(false);
		};

		append(layout);
	}
  } MsgBox;

  ThumbnailLayout layout;

  nall::png folderImg;
  nall::png fileImg;

  size_t numIcons;
  size_t MAX_ICONS;
  VerticalLayout iconsLayout[256];
  ImageIcon iconsImgs[256];
  nall::string iconsFilenameStrings[256];
  Label iconsFilename[256];
  nall::string currFolder;

  void loadFiles(const nall::string& filename) {
	  nall::string fullPath = {currFolder, "/", filename};
	  if (!nall::file::exists(fullPath) || !nall::directory::exists(fullPath)) {
		  return;
	  }
	  if (nall::directory::exists(fullPath)) {
		  //Directories repopulate the entire layout manager
		  layout.removeAll();
		  numIcons = 0;
		  lstring folders = nall::directory::folders(fullPath);
		  foreach(dirName, folders) {

		  }
		  lstring files = nall::directory::files(fullPath);
		  foreach(fileName, files) {
			  iconsImgs[numIcons].setImage(fileImg);
			  iconsFilenameStrings[numIcons] = fileName;
			  iconsFilename[numIcons].setText(iconsFilenameStrings[numIcons]);
			  layout.append(iconsLayout[numIcons]);
			  numIcons++;
		  }
	  } else if (nall::file::exists(fullPath)){
		  //Files simply open the message box
		  MsgBox.setVisible();
	  }

	  currFolder = fullPath;
  }

  void create() {
    //Do window tasks
    setTitle("Explorer");
    setGeometry({ 130, 130, 500, 400 });

    //Whee...
    MsgBox.create();

    //Initialize images
	if (!folderImg.decode("img/folder.png")) {
		std::cout <<"Couldn't load image.\n";
	}
	if (!fileImg.decode("img/file.png")) {
		std::cout <<"Couldn't load image.\n";
	}

    //Initialize layouts
	MAX_ICONS = 256;
    for (size_t i=0; i<MAX_ICONS; i++) {
    	iconsLayout[i].setAlignment(0.5);
    	iconsLayout[i].setMargin(5);
    	iconsImgs[i].setImage(fileImg);
    	iconsFilenameStrings[i] = "N/A";
    	iconsFilename[i].setText(iconsFilenameStrings[i]);
    	iconsLayout[i].append(iconsImgs[i], 0, 0);
    	iconsLayout[i].append(iconsFilename[i], 0, 0, 5);
    }
    numIcons = 0;
    currFolder = "";

    loadFiles("");

    //Initialize callbacks
    for (size_t i=0; i<MAX_ICONS; i++) {
    	iconsImgs[i].onMotion = [this, i](unsigned int x, unsigned int y, phoenix::MOVE_FLAG moveFlag) {
			if (moveFlag==phoenix::MOVE_FLAG::LEFT_UP) {
				loadFiles(iconsFilenameStrings[i]);
			}
    	};
    }

    //Master layout
    append(layout);

    onClose = &OS::quit;

    setVisible();
  }
} application;


int main(int argc, char* argv[])
{
  application.create();
  OS::main();
  return 0;
}



