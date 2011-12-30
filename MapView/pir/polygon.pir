#####################################################################
# Polygon class; wraps an sfml::Polygon* and provides cleanup on destruction
#####################################################################
.namespace ['Polygon']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a Polygon with args."
  throw $P0
.end

#Reclaim this polygon.
.sub 'cleanup' :method
  .param pmc ptr
  LIB_delete_item(ptr)
.end

#Retrieve the color of this polygon at a particular point
.sub 'get_point_color' :method
  .param int index
  .local pmc clr
  clr = LIB_dispatch_method(self, 'poly_get_point_color', 'ppi', index, 'wrap'=>'Color')
  .return(clr)
.end

#Set the color of this polygon at a specific index
.sub 'set_point_color' :method
  .param int index
  .param pmc color
  LIB_dispatch_method(self, 'poly_set_point_color', 'vpip', index, color)
.end

#Set this polygon's scale (x)
.sub 'set_scale_x' :method
  .param num scale
  LIB_dispatch_method(self, 'poly_set_scale_x', 'vpf', scale)
.end

#Set this polygon's scale (y)
.sub 'set_scale_y' :method
  .param num scale
  LIB_dispatch_method(self, 'poly_set_scale_y', 'vpf', scale)
.end

#Set this polygon's position (x)
.sub 'set_pos_x' :method
  .param int pos
  LIB_dispatch_method(self, 'poly_set_pos_x', 'vpi', pos)
.end

#Set this polygon's position (y)
.sub 'set_pos_y' :method
  .param int pos
  LIB_dispatch_method(self, 'poly_set_pos_y', 'vpi', pos)
.end

#Initialize this class.
.sub Polygon_class_init :anon :load :init
  $P0 = subclass 'Wrapped', 'Polygon'
.end


