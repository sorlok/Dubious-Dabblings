bool pCheckBox::checked() {
  return SendMessage(hwnd, BM_GETCHECK, 0, 0);
}

Geometry pCheckBox::minimumGeometry() {
  Font &font = this->font();
  Geometry geometry = font.geometry(checkBox.state.text);
  return { 0, 0, geometry.width + 20, font.p.height() + 4 };
}

void pCheckBox::setChecked(bool checked) {
  SendMessage(hwnd, BM_SETCHECK, (WPARAM)checked, 0);
}

void pCheckBox::setText(const string &text) {
  SetWindowText(hwnd, utf16_t(text));
}

void pCheckBox::constructor() {
  setWindow(Window::None);
}

void pCheckBox::setWindow(Window &window) {
  hwnd = CreateWindow(
    L"BUTTON", L"",
    WS_CHILD | WS_TABSTOP | BS_CHECKBOX,
    0, 0, 0, 0, window.p.hwnd, (HMENU)id, GetModuleHandle(0), 0
  );
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&checkBox);
  setDefaultFont();
  if(checkBox.state.checked) setChecked(true);
  setText(checkBox.state.text);
  synchronize();
}
