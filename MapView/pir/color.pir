#####################################################################
# Color class; wraps an sfml::Color* and provides cleanup on destruction
#####################################################################
.include "pir/lib.pir"
.include "pir/wrapped.pir"

.namespace ['Color']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a Color with args."
  throw $P0
.end

#Reclaim this color.
.sub 'cleanup' :method
  .param pmc ptr
  GAME_DeleteColor(ptr)
.end

#Modify the red value.
#TODO: This is pretty bad; can't we dispatch better?
.sub 'set_red' :method
  .param pmc red
  .local pmc lib, func, ptr

  #Retrieve the pointer
  $P0 = find_method self, 'get_ptr'
  ptr = self.$P0()

  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_red", "vpi"
  func(ptr, red)
.end

#Modify the green value.
.sub 'set_green' :method
  .param pmc green
  .local pmc lib, func, ptr

  #Retrieve the pointer
  $P0 = find_method self, 'get_ptr'
  ptr = self.$P0()

  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_green", "vpi"
  func(ptr, green)
.end

#Modify the blue value.
.sub 'set_blue' :method
  .param pmc blue
  .local pmc lib, func, ptr

  #Retrieve the pointer
  $P0 = find_method self, 'get_ptr'
  ptr = self.$P0()

  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_blue", "vpi"
  func(ptr, blue)
.end

#Modify the alpha value.
.sub 'set_alpha' :method
  .param pmc alpha
  .local pmc lib, func, ptr

  #Retrieve the pointer
  $P0 = find_method self, 'get_ptr'
  ptr = self.$P0()

  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_alpha", "vpi"
  func(ptr, alpha)
.end

#Initialize this class.
.sub Color_class_init :anon :load :init
  $P0 = subclass 'Wrapped', 'Color'
.end


