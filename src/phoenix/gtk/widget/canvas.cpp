static gboolean Canvas_expose(GtkWidget *widget, GdkEvent *event, pCanvas *self) {
  cairo_t *context = gdk_cairo_create(gtk_widget_get_window(widget));
  cairo_set_source_surface(context, self->surface, 0, 0);
  cairo_paint(context);
  cairo_destroy(context);
  return true;
}

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
  return (uint32_t*)cairo_image_surface_get_data(surface);
}

void pCanvas::setGeometry(const Geometry &geometry) {
  if(geometry.width  == cairo_image_surface_get_width(surface)
  && geometry.height == cairo_image_surface_get_height(surface)) return;

  cairo_surface_destroy(surface);
  surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, geometry.width, geometry.height);

  pWidget::setGeometry(geometry);
  update();
}

void pCanvas::update() {
  if(gtk_widget_get_realized(gtkWidget) == false) return;
  gdk_window_invalidate_rect(gtk_widget_get_window(gtkWidget), 0, true);
}

void pCanvas::constructor() {
  surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 256, 256);
  gtkWidget = gtk_drawing_area_new();
  GdkColor color;
  color.pixel = color.red = color.green = color.blue = 0;
  gtk_widget_modify_bg(gtkWidget, GTK_STATE_NORMAL, &color);
  gtk_widget_set_double_buffered(gtkWidget, false);
  gtk_widget_add_events(gtkWidget, GDK_EXPOSURE_MASK);
  g_signal_connect(G_OBJECT(gtkWidget), "expose_event", G_CALLBACK(Canvas_expose), (gpointer)this);

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

