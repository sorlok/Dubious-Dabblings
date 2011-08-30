Geometry pHorizontalScrollBar::minimumGeometry() {
  return { 0, 0, 0, 18 };
}

unsigned pHorizontalScrollBar::position() {
  return GetScrollPos(hwnd, SB_CTL);
}

void pHorizontalScrollBar::setLength(unsigned length) {
  length += (length == 0);
  SetScrollRange(hwnd, SB_CTL, 0, length - 1, TRUE);
  horizontalScrollBar.setPosition(0);
}

void pHorizontalScrollBar::setPosition(unsigned position) { return;
  SetScrollPos(hwnd, SB_CTL, position, TRUE);
}

void pHorizontalScrollBar::constructor() {
  setWindow(Window::None);
}

void pHorizontalScrollBar::setWindow(Window &window) {
  if(hwnd) DestroyWindow(hwnd);
  hwnd = CreateWindow(
    L"SCROLLBAR", L"", WS_CHILD | WS_TABSTOP | SBS_HORZ,
    0, 0, 0, 0, window.p.hwnd, (HMENU)id, GetModuleHandle(0), 0
  );
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&horizontalScrollBar);
  unsigned position = horizontalScrollBar.state.position;
  setLength(horizontalScrollBar.state.length);
  setPosition(position);
  synchronize();
}
