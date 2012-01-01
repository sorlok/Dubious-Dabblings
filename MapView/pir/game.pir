#####################################################################
# Game class; contains several useful points of control
#####################################################################
.namespace ['Game']

#Initialization code.
.sub 'init' :vtable
.end
.sub 'init_pmc' :vtable
  $P0 = new 'Exception'
  $P0 = "Currently can't create a Game with args."
  throw $P0
.end

#Initialize the engine. Returns non-zero for success.
.sub 'init_game' :method
  .param int width
  .param int height
  .param int depth

  null $P0
  $I0 = LIB_dispatch_method($P0, 'init_sfml', 'iiii', width, height, depth)
  .return($I0)
.end


#Process all events from the engine.
.sub 'process_events' :method
  null $P0
  $I0 = LIB_dispatch_method($P0, 'sfml_handle_events', 'i')
  .return($I0)
.end


#Close the game engine
.sub 'close' :method
  null $P0
  LIB_dispatch_method($P0, 'close_sfml', 'v')
.end

#Get the current frame time, in seconds
.sub 'get_frame_time_in_s' :method
  null $P0
  $N0 = LIB_dispatch_method($P0, 'game_get_frame_time_s', 'f')
  .return($N0)
.end

#Push all updates to the screen
.sub 'display' :method
  null $P0
  LIB_dispatch_method($P0, 'sfml_display', 'v')
.end

#Can we use PostFX
.sub 'can_use_pfx' :method
  null $P0
  $I0 = LIB_dispatch_method($P0, 'can_use_postfx', 'i')
  .return($I0)
.end

#Retrieve the window's width
.sub 'get_width' :method
  null $P0
  $I0 = LIB_dispatch_method($P0, 'game_get_width', 'i')
  .return($I0)
.end

#Retrieve the window's height
.sub 'get_height' :method
  null $P0
  $I0 = LIB_dispatch_method($P0, 'game_get_height', 'i')
  .return($I0)
.end

#Generic delete/draw stuff
.sub 'draw_item' :method
  .param pmc item

  #De-pointerize (if needed)
  #TODO: Can we do this with roles?
  $P1 = item
  $I0 = can item, 'get_ptr'
  unless $I0 goto nowdraw
  $P0 = find_method item, 'get_ptr'
  $P1 = item.$P0()

nowdraw:
  null $P0
  LIB_dispatch_method($P0, 'game_draw_item', 'vp', $P1)
.end


#Initialize this class.
.sub Game_class_init :anon :load :init
  $P0 = newclass 'Game'
.end


