#####################################################################
# Input class; provides an interface to sfml::Input
#####################################################################
.namespace ['Input']

#Initialization code.
.sub 'init' :vtable
  .local pmc ptr

  #Retrieve the current window's Input object.
  #ptr = LIB_dispatch_method(self, 'game_get_input', 'p')

  #Save it
  #$P0 = find_method self, 'set_ptr'
  #self.$P0(ptr)
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create an Input with args."
  throw $P0
.end

#Get the mouse's x coordinate
.sub 'get_mouse_x' :method
  null $P0
  $I0 = LIB_dispatch_method($P0, 'game_get_mouse_x', 'i')
  .return($I0)
.end

#Get the mouse's y coordinate
.sub 'get_mouse_y' :method
  null $P0
  $I0 = LIB_dispatch_method($P0, 'game_get_mouse_y', 'i')
  .return($I0)
.end

#Initialize this class.
.sub Input_class_init :anon :load :init
  $P0 = newclass 'Input'
.end


