static void Canvas_expose(pCanvas *self) {
  self->redraw();
}

//Added
static void Canvas_motion(Canvas *self, GdkEventMotion* event) {
  //Signal we are ready for the next mouse event.
  gint x, y;
  gdk_window_get_pointer(event->window, &x, &y, NULL);

  //Signal our user-defined function
  if(self->onMotion) self->onMotion(x, y, MOVE_FLAG::MOVE);
}

//Added
static void Canvas_enter(Canvas *self) {
  if(self->onMotion) self->onMotion(0, 0, MOVE_FLAG::ENTER);
}

static void Canvas_leave(Canvas *self) {
  if(self->onMotion) self->onMotion(0, 0, MOVE_FLAG::LEAVE);
}


uint32_t* pCanvas::buffer() {
  return bufferRGB;
}

const Geometry& pCanvas::bufferSize() {
  //Note: we could use gtkWidget->allocation.width/height, but I'm not taking any chances for now.
  return buffer_size;
}

void pCanvas::setGeometry(const Geometry &geometry) {
  delete[] bufferRGB;
  delete[] bufferBGR;

  bufferRGB = new uint32_t[geometry.width * geometry.height]();
  bufferBGR = new uint32_t[geometry.width * geometry.height]();
  buffer_size = geometry;

  pWidget::setGeometry(geometry);
  update();
}

void pCanvas::update() {
  if(gtk_widget_get_realized(gtkWidget) == false) return;
  GdkRectangle rect;
  rect.x = 0;
  rect.y = 0;
  rect.width = gtkWidget->allocation.width;
  rect.height = gtkWidget->allocation.height;
  gdk_window_invalidate_rect(gtkWidget->window, &rect, true);
}

void pCanvas::constructor() {
  bufferRGB = new uint32_t[256 * 256]();
  bufferBGR = new uint32_t[256 * 256]();
  buffer_size = {0, 0, 256, 256};

  gtkWidget = gtk_drawing_area_new();
  GdkColor color;
  color.pixel = color.red = color.green = color.blue = 0;
  gtk_widget_modify_bg(gtkWidget, GTK_STATE_NORMAL, &color);
  gtk_widget_set_double_buffered(gtkWidget, false);
  gtk_widget_add_events(gtkWidget, GDK_EXPOSURE_MASK);
  g_signal_connect_swapped(G_OBJECT(gtkWidget), "expose_event", G_CALLBACK(Canvas_expose), (gpointer)this);

  //ADDED: Mouse motion events
  //TODO:  Since these might be expensive, the user should explicitly call something like "add events"
  //       (which means we will also need a "remove events", and also need to check how QT and Windows do it)
  gtk_widget_add_events(gtkWidget, GDK_POINTER_MOTION_MASK|GDK_POINTER_MOTION_HINT_MASK);  //The hint mask slows down X's mouse events to one per update.
  g_signal_connect_swapped(G_OBJECT(gtkWidget), "motion_notify_event", G_CALLBACK(Canvas_motion), (gpointer)&canvas);

  //ADDED: Mouse enter/leaving events
  gtk_widget_add_events(gtkWidget, GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK);
  g_signal_connect_swapped(G_OBJECT(gtkWidget), "enter_notify_event", G_CALLBACK(Canvas_enter), (gpointer)&canvas);
  g_signal_connect_swapped(G_OBJECT(gtkWidget), "leave_notify_event", G_CALLBACK(Canvas_leave), (gpointer)&canvas);
}

void pCanvas::redraw() {
  if(gtk_widget_get_realized(gtkWidget) == false) return;
  uint32_t *rgb = bufferRGB, *bgr = bufferBGR;
  for(unsigned y = gtkWidget->allocation.height; y; y--) {
    for(unsigned x = gtkWidget->allocation.width; x; x--) {
      uint32_t pixel = *rgb++;
      *bgr++ = ((pixel << 16) & 0xff0000) | (pixel & 0x00ff00) | ((pixel >> 16) & 0x0000ff);
    }
  }

  gdk_draw_rgb_32_image(
    gtkWidget->window,
    gtkWidget->style->fg_gc[GTK_WIDGET_STATE(gtkWidget)],
    0, 0, gtkWidget->allocation.width, gtkWidget->allocation.height,
    GDK_RGB_DITHER_NONE, (guchar*)bufferBGR, sizeof(uint32_t) * gtkWidget->allocation.width
  );
}
