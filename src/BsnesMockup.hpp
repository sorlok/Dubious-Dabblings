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
  Label driverSel;
  Label focusSel;
  Label compositorSel;
  LineEdit statusBar;

  Label videoLbl;
  ComboBox videoCmb;
  Label audioLbl;
  ComboBox audioCmb;
  Label inputLbl;
  ComboBox inputCmb;

  RadioBox focusPause;
  RadioBox focusIgnore;
  RadioBox focusAllow;

  RadioBox compositNever;
  RadioBox compositFullscreen;
  RadioBox compositAlways;

  void create() {
    setTitle("BSNES Config Mockup");
    setGeometry({ 128, 128, 640, 480 });

    //Initialize components
    configPanels.append("Video");
    configPanels.append("Audio");
    configPanels.append("Input");
    configPanels.append("Advanced");
    configPanels.setSelection(3);
    statusBar.setText("(Pretend this is a status bar!)");
    statusBar.setEditable(false);
    settingsCaption.setText("Advanced Settings");
    settingsCaption.setFont("Arial, 20, Bold Italic");
    driverSel.setText("Driver selction:");
    driverSel.setFont("Arial, 10, Bold");
    videoLbl.setText("Video:");
    videoCmb.append("OpenGL");
    videoCmb.append("X-Video");
    videoCmb.append("SDL");
    videoCmb.append("None");
    videoCmb.setSelection(2);
    audioLbl.setText("Audio:");
    audioCmb.append("ALSA");
    audioCmb.append("OpenAL");
    audioCmb.append("OSS");
    audioCmb.append("PulseAudio");
    audioCmb.append("PulseAudioSimple");
    audioCmb.append("libao");
    audioCmb.append("None");
    inputLbl.setText("Input:");
    inputCmb.append("SDL");
    inputCmb.append("X-Windows");
    inputCmb.append("None");
    focusSel.setText("When emulation window lacks focus:");
    focusSel.setFont("Arial, 10, Bold");
    focusPause.setText("Pause emulator");
    focusIgnore.setText("Ignore input");
    focusAllow.setText("Allow input");
    RadioBox::group(focusPause, focusIgnore, focusAllow);
    focusPause.setChecked();
    compositorSel.setText("Disable window compositor:");
    compositorSel.setFont("Arial, 10, Bold");
    compositNever.setText("Never");
    compositFullscreen.setText("Fullscreen");
    compositAlways.setText("Always");
    RadioBox::group(compositNever, compositFullscreen, compositAlways);
    compositFullscreen.setChecked();

    //Perform layout
    layout.append(configPanels, {0.0, 5}, {0.0, 5}, {configPanels, 120}, {statusBar, -5});
    layout.append(statusBar, {0.0, 5}, {statusBar, -20, ANCHOR::BOTTOM}, {1.0, -5}, {1.0, -5});
    layout.append(sublayout, {configPanels, 10}, {0.0}, {1.0}, {1.0});
    sublayout.append(settingsCaption, {0.0, 5}, {0.0, 5});
    sublayout.append(driverSel, {settingsCaption, 0, ANCHOR::LEFT}, {settingsCaption, 10});
    sublayout.append(videoLbl, {0.0, 5}, {videoCmb, 5, ANCHOR::TOP});
    sublayout.append(videoCmb, {videoLbl, 5}, {driverSel, 5}, {0.333, -5});
    sublayout.append(audioLbl, {0.333, 5}, {audioCmb, 5, ANCHOR::TOP});
    sublayout.append(audioCmb, {audioLbl, 5}, {driverSel, 5}, {0.667, -5});
    sublayout.append(inputLbl, {0.667, 5}, {inputCmb, 5, ANCHOR::TOP});
    sublayout.append(inputCmb, {inputLbl, 5}, {driverSel, 5}, {1.0, -5});
    sublayout.append(focusSel, {settingsCaption, 0, ANCHOR::LEFT}, {videoCmb, 15});
    sublayout.append(focusPause, {0.0, 5}, {focusSel, 5});
    sublayout.append(focusIgnore, {0.333, 5}, {focusSel, 5});
    sublayout.append(focusAllow, {0.667, 5}, {focusSel, 5});
    sublayout.append(compositorSel, {settingsCaption, 0, ANCHOR::LEFT}, {focusPause, 15});
    sublayout.append(compositNever, {0.0, 5}, {compositorSel, 5});
    sublayout.append(compositFullscreen, {0.333, 5}, {compositorSel, 5});
    sublayout.append(compositAlways, {0.667, 5}, {compositorSel, 5});

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
