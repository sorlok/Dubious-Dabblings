struct pFont;
struct pWindow;
struct pMenu;
struct pLayout;
struct pWidget;

struct pObject {
  Object &object;
  uintptr_t id;
  bool locked;
  static array<pObject*> objects;

  pObject(Object &object);
  static pObject* find(unsigned id);
  virtual ~pObject() {}

  void constructor() {}
  void destructor() {}
};

struct pOS : public pObject {
  struct State {
    Font defaultFont;
  };
  static State *state;

  static Geometry availableGeometry();
  static Geometry desktopGeometry();
  static string fileLoad(Window &parent, const string &path, const lstring &filter);
  static string fileSave(Window &parent, const string &path, const lstring &filter);
  static string folderSelect(Window &parent, const string &path);
  static void main();
  static bool pendingEvents();
  static void processEvents();
  static void quit();

  static void initialize();
};

struct pFont : public pObject {
  Font &font;
  HFONT hfont;

  Geometry geometry(const string &text);
  unsigned height();
  void setBold(bool bold);
  void setFamily(const string &family);
  void setItalic(bool italic);
  void setSize(unsigned size);
  void setUnderline(bool underline);

  pFont(Font &font) : pObject(font), font(font) {}
  void constructor();
};

struct pTimer : public pObject {
  Timer &timer;
  UINT_PTR htimer;

  void setEnabled(bool enabled);
  void setInterval(unsigned milliseconds);

  pTimer(Timer &timer) : pObject(timer), timer(timer) {}
  void constructor();
};

struct pMessageWindow : public pObject {
  static MessageWindow::Response information(Window &parent, const string &text, MessageWindow::Buttons buttons);
  static MessageWindow::Response question(Window &parent, const string &text, MessageWindow::Buttons buttons);
  static MessageWindow::Response warning(Window &parent, const string &text, MessageWindow::Buttons buttons);
  static MessageWindow::Response critical(Window &parent, const string &text, MessageWindow::Buttons buttons);
};

struct pWindow : public pObject {
  Window &window;
  HWND hwnd;
  HMENU hmenu;
  HWND hstatus;
  HBRUSH brush;
  COLORREF brushColor;

  void append(Layout &layout);
  void append(Menu &menu);
  void append(Widget &widget);
  Color backgroundColor();
  bool focused();
  Geometry frameMargin();
  Geometry geometry();
  void remove(Layout &layout);
  void remove(Menu &menu);
  void remove(Widget &widget);
  void setBackgroundColor(const Color &color);
  void setFocused();
  void setFullScreen(bool fullScreen);
  void setGeometry(const Geometry &geometry);
  void setMenuFont(Font &font);
  void setMenuVisible(bool visible);
  void setResizable(bool resizable);
  void setStatusFont(Font &font);
  void setStatusText(const string &text);
  void setStatusVisible(bool visible);
  void setTitle(const string &text);
  void setVisible(bool visible);
  void setWidgetFont(Font &font);

  pWindow(Window &window) : pObject(window), window(window) {}
  void constructor();
  void updateMenu();
};

struct pAction : public pObject {
  Action &action;
  Menu *parentMenu;
  Window *parentWindow;

  void setEnabled(bool enabled);
  void setVisible(bool visible);

  pAction(Action &action) : pObject(action), action(action) {}
  void constructor();
};

struct pMenu : public pAction {
  Menu &menu;
  HMENU hmenu;

  void append(Action &action);
  void remove(Action &action);
  void setText(const string &text);

  pMenu(Menu &menu) : pAction(menu), menu(menu) {}
  void constructor();
  void destructor();
  void update(Window &parentWindow, Menu *parentMenu = 0);
};

struct pSeparator : public pAction {
  Separator &separator;

  pSeparator(Separator &separator) : pAction(separator), separator(separator) {}
  void constructor();
  void destructor();
};

struct pItem : public pAction {
  Item &item;

  void setText(const string &text);

  pItem(Item &item) : pAction(item), item(item) {}
  void constructor();
  void destructor();
};

struct pCheckItem : public pAction {
  CheckItem &checkItem;

  bool checked();
  void setChecked(bool checked);
  void setText(const string &text);

  pCheckItem(CheckItem &checkItem) : pAction(checkItem), checkItem(checkItem) {}
  void constructor();
  void destructor();
};

struct pRadioItem : public pAction {
  RadioItem &radioItem;

  bool checked();
  void setChecked();
  void setGroup(const reference_array<RadioItem&> &group);
  void setText(const string &text);

  pRadioItem(RadioItem &radioItem) : pAction(radioItem), radioItem(radioItem) {}
  void constructor();
  void destructor();
};

struct pSizable : public pObject {
  Sizable &sizable;

  pSizable(Sizable &sizable) : pObject(sizable), sizable(sizable) {}
};

struct pLayout : public pSizable {
  Layout &layout;

  pLayout(Layout &layout) : pSizable(layout), layout(layout) {}
};

struct pWidget : public pSizable {
  Widget &widget;
  HWND hwnd;

  bool enabled();
  Font& font();
  virtual Geometry minimumGeometry();
  void setEnabled(bool enabled);
  void setFocused();
  void setFont(Font &font);
  virtual void setGeometry(const Geometry &geometry);
  void setVisible(bool visible);
  virtual void setWindow(Window &window);

  pWidget(Widget &widget) : pSizable(widget), widget(widget) {}
  void constructor();
  void setDefaultFont();
  void synchronize();
};

struct pButton : public pWidget {
  Button &button;

  Geometry minimumGeometry();
  void setText(const string &text);

  pButton(Button &button) : pWidget(button), button(button) {}
  void constructor();
  void setWindow(Window &window);
};

struct pCanvas : public pWidget {
  Canvas &canvas;
  uint32_t *bufferRGB;

  uint32_t* buffer();
  void setGeometry(const Geometry &geometry);
  void update();

  pCanvas(Canvas &canvas) : pWidget(canvas), canvas(canvas) {}
  void constructor();
  void setWindow(Window &window);
};

struct pCheckBox : public pWidget {
  CheckBox &checkBox;

  bool checked();
  Geometry minimumGeometry();
  void setChecked(bool checked);
  void setText(const string &text);

  pCheckBox(CheckBox &checkBox) : pWidget(checkBox), checkBox(checkBox) {}
  void constructor();
  void setWindow(Window &window);
};

struct pComboBox : public pWidget {
  ComboBox &comboBox;

  void append(const string &text);
  Geometry minimumGeometry();
  void reset();
  unsigned selection();
  void setSelection(unsigned row);

  pComboBox(ComboBox &comboBox) : pWidget(comboBox), comboBox(comboBox) {}
  void constructor();
  void setGeometry(const Geometry &geometry);
  void setWindow(Window &window);
};

struct pHexEdit : public pWidget {
  HexEdit &hexEdit;
  LRESULT CALLBACK (*windowProc)(HWND, UINT, LPARAM, WPARAM);

  void setColumns(unsigned columns);
  void setLength(unsigned length);
  void setOffset(unsigned offset);
  void setRows(unsigned rows);
  void update();

  pHexEdit(HexEdit &hexEdit) : pWidget(hexEdit), hexEdit(hexEdit) {}
  void constructor();
  bool keyPress(unsigned key);
  void setWindow(Window &window);
};

struct pHorizontalScrollBar : public pWidget {
  HorizontalScrollBar &horizontalScrollBar;

  Geometry minimumGeometry();
  unsigned position();
  void setLength(unsigned length);
  void setPosition(unsigned position);

  pHorizontalScrollBar(HorizontalScrollBar &horizontalScrollBar) : pWidget(horizontalScrollBar), horizontalScrollBar(horizontalScrollBar) {}
  void constructor();
  void setWindow(Window &window);
};

struct pHorizontalSlider : public pWidget {
  HorizontalSlider &horizontalSlider;

  Geometry minimumGeometry();
  unsigned position();
  void setLength(unsigned length);
  void setPosition(unsigned position);

  pHorizontalSlider(HorizontalSlider &horizontalSlider) : pWidget(horizontalSlider), horizontalSlider(horizontalSlider) {}
  void constructor();
  void setWindow(Window &window);
};

struct pLabel : public pWidget {
  Label &label;

  Geometry minimumGeometry();
  void setText(const string &text);

  pLabel(Label &label) : pWidget(label), label(label) {}
  void constructor();
  void setWindow(Window &window);
};

struct pLineEdit : public pWidget {
  LineEdit &lineEdit;

  Geometry minimumGeometry();
  void setEditable(bool editable);
  void setText(const string &text);
  string text();

  pLineEdit(LineEdit &lineEdit) : pWidget(lineEdit), lineEdit(lineEdit) {}
  void constructor();
  void setWindow(Window &window);
};

struct pListView : public pWidget {
  ListView &listView;
  bool lostFocus;

  void append(const lstring &text);
  void autoSizeColumns();
  bool checked(unsigned row);
  void modify(unsigned row, const lstring &text);
  void reset();
  bool selected();
  unsigned selection();
  void setCheckable(bool checkable);
  void setChecked(unsigned row, bool checked);
  void setHeaderText(const lstring &text);
  void setHeaderVisible(bool visible);
  void setSelected(bool selected);
  void setSelection(unsigned row);

  pListView(ListView &listView) : pWidget(listView), listView(listView) {}
  void constructor();
  void setGeometry(const Geometry &geometry);
  void setWindow(Window &window);
};

struct pProgressBar : public pWidget {
  ProgressBar &progressBar;

  Geometry minimumGeometry();
  void setPosition(unsigned position);

  pProgressBar(ProgressBar &progressBar) : pWidget(progressBar), progressBar(progressBar) {}
  void constructor();
  void setWindow(Window &window);
};

struct pRadioBox : public pWidget {
  RadioBox &radioBox;

  bool checked();
  Geometry minimumGeometry();
  void setChecked();
  void setGroup(const reference_array<RadioBox&> &group);
  void setText(const string &text);

  pRadioBox(RadioBox &radioBox) : pWidget(radioBox), radioBox(radioBox) {}
  void constructor();
  void setWindow(Window &window);
};

struct pTextEdit : public pWidget {
  TextEdit &textEdit;

  void setCursorPosition(unsigned position);
  void setEditable(bool editable);
  void setText(const string &text);
  void setWordWrap(bool wordWrap);
  string text();

  pTextEdit(TextEdit &textEdit) : pWidget(textEdit), textEdit(textEdit) {}
  void constructor();
  void setWindow(Window &window);
};

struct pVerticalScrollBar : public pWidget {
  VerticalScrollBar &verticalScrollBar;

  Geometry minimumGeometry();
  unsigned position();
  void setLength(unsigned length);
  void setPosition(unsigned position);

  pVerticalScrollBar(VerticalScrollBar &verticalScrollBar) : pWidget(verticalScrollBar), verticalScrollBar(verticalScrollBar) {}
  void constructor();
  void setWindow(Window &window);
};

struct pVerticalSlider : public pWidget {
  VerticalSlider &verticalSlider;

  Geometry minimumGeometry();
  unsigned position();
  void setLength(unsigned length);
  void setPosition(unsigned position);

  pVerticalSlider(VerticalSlider &verticalSlider) : pWidget(verticalSlider), verticalSlider(verticalSlider) {}
  void constructor();
  void setWindow(Window &window);
};

struct pViewport : public pWidget {
  Viewport &viewport;

  uintptr_t handle();

  pViewport(Viewport &viewport) : pWidget(viewport), viewport(viewport) {}
  void constructor();
  void setWindow(Window &window);
};
