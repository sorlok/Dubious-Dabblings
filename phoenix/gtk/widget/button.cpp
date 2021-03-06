static void Button_activate(Button *self) {
  if(self->onActivate) self->onActivate();
}

Geometry pButton::minimumGeometry() {
  Geometry geometry = pFont::geometry(widget.state.font, button.state.text);
  return { 0, 0, geometry.width + 24, geometry.height + 12 };
}

void pButton::setText(const string &text) {
  gtk_button_set_label(GTK_BUTTON(gtkWidget), text);
  setFont(widget.state.font);
}

void pButton::constructor() {
  gtkWidget = gtk_button_new();
  g_signal_connect_swapped(G_OBJECT(gtkWidget), "clicked", G_CALLBACK(Button_activate), (gpointer)&button);

  setText(button.state.text);
}

void pButton::destructor() {
  gtk_widget_destroy(gtkWidget);
}

void pButton::orphan() {
  destructor();
  constructor();
}
