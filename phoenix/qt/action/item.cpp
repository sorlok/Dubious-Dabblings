void pItem::setText(const string &text) {
  qtAction->setText(QString::fromUtf8(text));
}

void pItem::constructor() {
  qtAction = new QAction(0);
  connect(qtAction, SIGNAL(triggered()), SLOT(onTick()));
}

void pItem::destructor() {
  if(action.state.menu) action.state.menu->remove(item);
  delete qtAction;
}

void pItem::onTick() {
  if(item.onTick) item.onTick();
}
