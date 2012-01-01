#####################################################################
# Sprite class; wraps an sfml::Sprite* and provides cleanup on destruction
#####################################################################
.namespace ['Sprite']

#Initialization code.
.sub 'init' :vtable
  .local pmc ptr

  null $P0
  ptr = LIB_dispatch_method($P0, 'new_sprite', 'p')
  $P0 = find_method self, 'set_ptr'
  self.$P0(ptr)
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a Sprite with args."
  throw $P0
.end

#Reclaim this sprite.
.sub 'cleanup' :method
  .param pmc ptr
  LIB_delete_item(ptr)
.end

#Set the image for this sprite
.sub 'set_image' :method
  .param pmc img
  LIB_dispatch_method(self, 'sprite_set_image', 'vpp', img)
.end

#Turn off alpha blending
.sub 'set_blend_off' :method
  LIB_dispatch_method(self, 'sprite_set_blend_off', 'vp')
.end

#Designate a sub-rectangle of this image to be used for display
.sub 'set_sub_rect' :method
  .param int x
  .param int y
  .param int width
  .param int height
  LIB_dispatch_method(self, 'sprite_set_sub_rect', 'vpiiii', x, y, width, height)
.end

#Retrieve sub-rectangle: x
.sub 'get_sub_rect_x' :method
  $I0 = LIB_dispatch_method(self, 'sprite_get_sub_rect_x', 'ip')
  .return($I0)
.end

#Retrieve sub-rectangle: y
.sub 'get_sub_rect_y' :method
  $I0 = LIB_dispatch_method(self, 'sprite_get_sub_rect_y', 'ip')
  .return($I0)
.end

#Set this sprite's position (x,y)
.sub 'set_position' :method
  .param int x
  .param int y
  LIB_dispatch_method(self, 'sprite_set_position', 'vpii', x, y)
.end

#Set this sprite's center (x,y)
.sub 'set_center' :method
  .param int x
  .param int y
  LIB_dispatch_method(self, 'sprite_set_center', 'vpii', x, y)
.end

#Get this sprite's rotation
.sub 'get_rotation' :method
  .local num retVal
  retVal = LIB_dispatch_method(self, 'sprite_get_rotation', 'fp')
  .return(retVal)
.end

#Set this sprite's rotation
.sub 'set_rotation' :method
  .param num angle
  LIB_dispatch_method(self, 'sprite_set_rotation', 'vpf', angle)
.end

#Get this sprite's color
.sub 'get_color' :method
  .local pmc clr
  clr = LIB_dispatch_method(self, 'sprite_get_color', 'pp', 'wrap'=>'Color')
  .return(clr)
.end

#Set this sprite's color
.sub 'set_color' :method
  .param pmc color
  LIB_dispatch_method(self, 'sprite_set_color', 'vpp', color)
.end

#Initialize this class.
.sub Sprite_class_init :anon :load :init
  $P0 = subclass 'Wrapped', 'Sprite'
.end


