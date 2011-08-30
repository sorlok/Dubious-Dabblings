uintptr_t pViewport::handle() {
  return (uintptr_t)hwnd;
}

void pViewport::constructor() {
  setWindow(Window::None);
}

void pViewport::setWindow(Window &window) {
  hwnd = CreateWindow(L"phoenix_viewport", L"", WS_CHILD | WS_DISABLED, 0, 0, 0, 0, window.p.hwnd, (HMENU)id, GetModuleHandle(0), 0);
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&viewport);
  synchronize();
}

static LRESULT CALLBACK Viewport_windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  return DefWindowProc(hwnd, msg, wparam, lparam);
}
