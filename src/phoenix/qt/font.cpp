Geometry pFont::geometry(const string &text) {
  if(qtFont == 0) return { 0, 0, 0, 0 };

  QFontMetrics metrics(*qtFont);

  lstring lines;
  lines.split("\n", text);

  unsigned maxWidth = 0;
  foreach(line, lines) {
    maxWidth = max(maxWidth, metrics.width(line));
  }

  return { 0, 0, maxWidth, metrics.height() * lines.size() };
}

void pFont::setBold(bool bold) { update(); }
void pFont::setFamily(const string &family) { update(); }
void pFont::setItalic(bool italic) { update(); }
void pFont::setSize(unsigned size) { update(); }
void pFont::setUnderline(bool underline) { update(); }

void pFont::constructor() {
  qtFont = new QFont;
  font.setFamily("Sans");
  font.setSize(8);
}

void pFont::destructor() {
  delete qtFont;
  qtFont = 0;
}

void pFont::update() {
  if(qtFont == 0) return;

  qtFont->setFamily(QString::fromUtf8(font.state.family));
  qtFont->setPointSize(font.state.size);
  qtFont->setBold(font.state.bold);
  qtFont->setItalic(font.state.italic);
  qtFont->setUnderline(font.state.underline);
}
