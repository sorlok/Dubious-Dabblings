Font& pWidget::font() {
  if(widget.state.font) return *widget.state.font;
  return pOS::defaultFont;
}

Geometry pWidget::minimumGeometry() {
  return { 0, 0, 0, 0 };
}

void pWidget::setEnabled(bool enabled) {
  if(widget.state.abstract) enabled = false;
  if(sizable.state.layout && sizable.state.layout->enabled() == false) enabled = false;
  qtWidget->setEnabled(enabled);
}

void pWidget::setFocused() {
  qtWidget->setFocus(Qt::OtherFocusReason);
}

void pWidget::setFont(Font &font) {
  if(font.p.qtFont) qtWidget->setFont(*font.p.qtFont);
  else qtWidget->setFont(*pOS::defaultFont.p.qtFont);
}

void pWidget::setGeometry(const Geometry &geometry) {
  qtWidget->setGeometry(geometry.x, geometry.y, geometry.width, geometry.height);
}

void pWidget::setVisible(bool visible) {
  if(widget.state.abstract) visible = false;
  if(sizable.state.layout && sizable.state.layout->visible() == false) visible = false;
  qtWidget->setVisible(visible);
}

void pWidget::constructor() {
  if(widget.state.abstract) qtWidget = new QWidget;
}

void pWidget::destructor() {
  if(widget.state.abstract) delete qtWidget;
}
