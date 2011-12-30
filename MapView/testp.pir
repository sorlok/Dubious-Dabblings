###Main library code.

#More stuff
.include "pir/lib.pir"
.include "pir/wrapped.pir"
.include "pir/color.pir"
.include "pir/input.pir"
.include "pir/image.pir"
.include "pir/polygon.pir"

#####################################################################
# Base classes, to be subclassed by our library.
#####################################################################

.namespace ['Rendition']
.sub 'init' :vtable
.end

.sub 'update' :method 
.end

.sub 'display' :method
.end

.sub Rendition_class_init :anon :load :init
    newclass $P0, [ 'Rendition' ]
.end



#####################################################################
# Shared library functions. Generally PIR need maintain nothing more 
#   than a function pointer; no struct wrapping is required. 
#####################################################################

.namespace[]

#Initialize the engine. 
.sub 'GAME_Init'
  .param int width
  .param int height
  .param int depth
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "init_sfml", "iiii"
  $I0 = func(width, height, depth)
  .return($I0)
.end


#Process all events from the engine.
.sub 'GAME_ProcessEvents'
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "sfml_handle_events", "i"
  $I0 = func()
  .return($I0)
.end


#Close the game engine
.sub 'GAME_Close'
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "close_sfml", "v"
  func()
.end


.sub 'GAME_GetFrameTimeInS'
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "game_get_frame_time_s", "f"
  $N0 = func()
  .return($N0)
.end



#Sample code to draw objects from SampleUpdate. To be removed.
.sub 'DEMO_SampleDisplay'
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "demo_display", "v"
  func()
.end

#DEMO: Create a polygon
.sub 'DEMO_InitPoly'
  .local pmc ptr, poly

  #Make it
  null $P0
  ptr = LIB_dispatch_method($P0, 'demo_init_poly', 'p')
  poly = new 'Polygon'

  #Save it
  $P0 = find_method poly, 'set_ptr'
  poly.$P0(ptr)

  .return(poly)
.end


#Push all updates to the screen
.sub 'GAME_Display'
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "sfml_display", "v"
  func()
.end






#PostFX
.sub 'PFX_CanUse'
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "can_use_postfx", "i"
  $I0 = func()
  .return($I0)
.end


.sub 'PFX_MakeNew'
  .param string filename
  .local pmc lib, func, bb

  #Convert the string to a byte buffer
  bb = new ['ByteBuffer']
  bb = filename
  push bb, 0

  lib = LIB_get_dll()
  func = dlfunc lib, "new_postfx", "pp"
  $P0 = func(bb)

  .return($P0)
.end


.sub 'DEMO_SetDefaultPFX'
  .param pmc item
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "demo_set_default_postfx", "vp"
  func(item)
.end

.sub 'DEMO_UpdatePFXColor'
  .param pmc item
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "demo_update_postfx_color", "vp"
  func(item)
.end


#Sprite
.sub 'SPR_MakeNew'
  .local pmc lib, func

  lib = LIB_get_dll()
  func = dlfunc lib, "new_sprite", "p"
  $P0 = func()

  .return($P0)
.end

.sub 'SPR_SetImage'
  .param pmc item
  .param pmc img
  .local pmc lib, func
  $P0 = img.'get_ptr'()

  lib = LIB_get_dll()
  func = dlfunc lib, "sprite_set_image", "vpp"
  func(item, $P0)
.end

.sub 'SPR_SetPosition'
  .param pmc item
  .param int x
  .param int y
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "sprite_set_position", "vpii"
  func(item, x, y)
.end

.sub 'SPR_SetCenter'
  .param pmc item
  .param int x
  .param int y
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "sprite_set_center", "vpii"
  func(item, x, y)
.end


.sub 'SPR_GetRotation'
  .param pmc item
  .local pmc lib, func
  .local num retVal
  lib = LIB_get_dll()
  func = dlfunc lib, "sprite_get_rotation", "fp"
  retVal = func(item)
  .return(retVal)
.end


.sub 'SPR_SetRotation'
  .param pmc item
  .param num angle
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "sprite_set_rotation", "vpf"
  func(item, angle)
.end


.sub 'SPR_GetColor'
  .param pmc item
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "sprite_get_color", "pp"
  $P0 = func(item)

  #Test
  $P1  = new 'Color'
  $P1.'set_ptr'($P0)
  #Subclass isn't working...

  .return($P1)
.end


.sub 'SPR_SetColor'
  .param pmc item
  .param pmc color
  .local pmc lib, func
  $P0 = color.'get_ptr'()

  lib = LIB_get_dll()
  func = dlfunc lib, "sprite_set_color", "vpp"
  func(item, $P0)
.end



#Generic delete/draw stuff
.sub 'GAME_DrawItem'
  .param pmc item
  .local pmc lib, func

  #De-pointerize (if needed)
  $P1 = item
  $I0 = can item, 'get_ptr'
  unless $I0 goto nowdraw
  $P0 = find_method item, 'get_ptr'
  $P1 = item.$P0()

nowdraw:
  lib = LIB_get_dll()
  func = dlfunc lib, "game_draw_item", "vp"
  func($P1)
.end


.sub 'GAME_DeleteItem'
  .param pmc item
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "game_del_item", "vp"
  func(item)
.end

.sub 'GAME_DeleteColor'
  .param pmc item
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "game_del_color", "vp"
  func(item)
.end

.sub 'GAME_DeleteImage'
  .param pmc item
  .local pmc lib, func
  lib = LIB_get_dll()
  func = dlfunc lib, "game_del_image", "vp"
  func(item)
.end



#####################################################################
# Main game loop. Makes use of shared library code where possible.
#####################################################################

.namespace[]
.sub 'run_game'
    .local int res
    .param pmc currRend

    #Start the game
    res = GAME_Init(800, 600, 32)
    if res==0 goto done

    say "Starting main loop"

    #Main Loop
    main_loop:
      #Have our game handle expected events
      $I0 = GAME_ProcessEvents()

      #Now update within the game loop
      #DEMO_SampleUpdate()

      #Call our sample game object's update method
      currRend.'update'()

      #Display what we've just rendered
      DEMO_SampleDisplay()

      #And here is where the real updating happens
      currRend.'display'()

      #Here we go
      GAME_Display()

      #Continue to update
      if $I0==0 goto main_loop

    #When we're done, clean up
    GAME_Close()

    done:
      say "Done"
.end




