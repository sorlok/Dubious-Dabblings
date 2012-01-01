#####################################################################
# Color class; wraps an sfml::Color* and provides cleanup on destruction
#####################################################################
.namespace ['Color']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a Color with args."
  throw $P0
.end

#Make a new color
.sub 'init_new' :method
  .param int red
  .param int green
  .param int blue
  .local pmc ptr

  null $P0
  ptr = LIB_dispatch_method($P0, 'new_color', 'piii', red, green, blue)
  $P0 = find_method self, 'set_ptr'
  self.$P0(ptr)
.end

#Reclaim this color.
.sub 'cleanup' :method
  .param pmc ptr
  LIB_delete_color(ptr)
.end

#Modify the red value.
.sub 'set_red' :method
  .param pmc red
  LIB_dispatch_method(self, 'color_set_red', 'vpi', red)
.end

#Modify the green value.
.sub 'set_green' :method
  .param pmc green
  LIB_dispatch_method(self, 'color_set_green', 'vpi', green)
.end

#Modify the blue value.
.sub 'set_blue' :method
  .param pmc blue
  LIB_dispatch_method(self, 'color_set_blue', 'vpi', blue)
.end

#Modify the alpha value.
.sub 'set_alpha' :method
  .param pmc alpha
  LIB_dispatch_method(self, 'color_set_alpha', 'vpi', alpha)
.end

#Initialize this class.
.sub Color_class_init :anon :load :init
  $P0 = subclass 'Wrapped', 'Color'
.end


