bool pWidget::enabled() {
  return IsWindowEnabled(hwnd);
}

Font& pWidget::font() {
  if(widget.state.font) return *widget.state.font;
  return pOS::state->defaultFont;
}

Geometry pWidget::minimumGeometry() {
  return { 0, 0, 0, 0 };
}

void pWidget::setEnabled(bool enabled) {
  if(widget.state.abstract) enabled = false;
  if(sizable.state.layout && sizable.state.layout->enabled() == false) enabled = false;
  EnableWindow(hwnd, enabled);
}

void pWidget::setFocused() {
  SetFocus(hwnd);
}

void pWidget::setFont(Font &font) {
  SendMessage(hwnd, WM_SETFONT, (WPARAM)font.p.hfont, 0);
}

void pWidget::setGeometry(const Geometry &geometry) {
  SetWindowPos(hwnd, NULL, geometry.x, geometry.y, geometry.width, geometry.height, SWP_NOZORDER);
}

void pWidget::setWindow(Window &window) {
  if(hwnd) DestroyWindow(hwnd);
  hwnd = CreateWindow(L"phoenix_label", L"", WS_CHILD, 0, 0, 0, 0, window.p.hwnd, (HMENU)id, GetModuleHandle(0), 0);
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&widget);
}

void pWidget::setVisible(bool visible) {
  if(widget.state.abstract) visible = false;
  if(sizable.state.layout && sizable.state.layout->visible() == false) visible = false;
  ShowWindow(hwnd, visible ? SW_SHOWNORMAL : SW_HIDE);
}

void pWidget::constructor() {
  hwnd = 0;
  if(widget.state.abstract) setWindow(Window::None);
}

void pWidget::setDefaultFont() {
  if(widget.state.font) {
    SendMessage(hwnd, WM_SETFONT, (WPARAM)widget.state.font->p.hfont, 0);
  } else {
    SendMessage(hwnd, WM_SETFONT, (WPARAM)pOS::state->defaultFont.p.hfont, 0);
  }
}

//calling Widget::setParent destroys widget and re-creates it:
//need to re-apply visiblity and enabled status; called by each subclassed setParent() function
void pWidget::synchronize() {
  widget.setEnabled(widget.enabled());
  widget.setVisible(widget.visible());
}
