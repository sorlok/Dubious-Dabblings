###Main library code.

#More stuff
.include "pir/color.pir"

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

#Internal function: Get the DLL
.sub 'INT_GetDLL'
  .local pmc lib

  #Already loaded once
  get_hll_global lib, 'library_pmc'
  unless null lib goto end

  #Loading for the first time
  lib = loadlib "/home/sethhetu/dubious/MapView/libsfml_engine.so"
  set_hll_global 'library_pmc', lib
  if lib goto end
  
  #Error
  $P0 = new 'Exception'
  $P0 = "Couldn't find library!"
  throw $P0

  end:
  .return(lib)
.end

#Initialize the engine. 
.sub 'GAME_Init'
  .param int width
  .param int height
  .param int depth
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "init_sfml", "iiii"
  $I0 = func(width, height, depth)
  .return($I0)
.end


#Process all events from the engine.
.sub 'GAME_ProcessEvents'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "sfml_handle_events", "i"
  $I0 = func()
  .return($I0)
.end


#Close the game engine
.sub 'GAME_Close'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "close_sfml", "v"
  func()
.end


#Retrieve current mouse x/y
.sub 'INPUT_GetMouseX'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "game_get_mouse_x", "i"
  $I0 = func()
  .return($I0)
.end
.sub 'INPUT_GetMouseY'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "game_get_mouse_y", "i"
  $I0 = func()
  .return($I0)
.end


.sub 'GAME_GetFrameTimeInS'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "game_get_frame_time_s", "f"
  $N0 = func()
  .return($N0)
.end



#Sample code to draw objects from SampleUpdate. To be removed.
.sub 'DEMO_SampleDisplay'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "demo_display", "v"
  func()
.end

#Create a polygon
.sub 'DEMO_InitPoly'
  .local pmc lib, func

  lib = INT_GetDLL()
  func = dlfunc lib, "demo_init_poly", "p"
  $P0 = func()

  .return($P0)
.end


#Push all updates to the screen
.sub 'GAME_Display'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "sfml_display", "v"
  func()
.end



#Update our "poly" position with the given x/y
.sub 'DEMO_SetPolyPos'
  .param int x
  .param int y
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "game_set_poly_pos", "vii"
  func(x, y)
.end


#PostFX
.sub 'PFX_CanUse'
  .local pmc lib, func
  lib = INT_GetDLL()
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

  lib = INT_GetDLL()
  func = dlfunc lib, "new_postfx", "pp"
  $P0 = func(bb)

  .return($P0)
.end


.sub 'DEMO_SetDefaultPFX'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "demo_set_default_postfx", "vp"
  func(item)
.end

.sub 'DEMO_UpdatePFXColor'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "demo_update_postfx_color", "vp"
  func(item)
.end



#Image
.sub 'IMG_MakeNew'
  .param string filename
  .local pmc lib, func, bb

  #Convert the string to a byte buffer
  bb = new ['ByteBuffer']
  bb = filename
  push bb, 0

  lib = INT_GetDLL()
  func = dlfunc lib, "new_image", "pp"
  $P0 = func(bb)

  .return($P0)
.end

.sub 'IMG_GetWidth'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "image_get_width", "ip"
  $I0 = func(item)
  .return($I0)
.end

.sub 'IMG_GetHeight'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "image_get_height", "ip"
  $I0 = func(item)
  .return($I0)
.end


#Sprite
.sub 'SPR_MakeNew'
  .local pmc lib, func

  lib = INT_GetDLL()
  func = dlfunc lib, "new_sprite", "p"
  $P0 = func()

  .return($P0)
.end

.sub 'SPR_SetImage'
  .param pmc item
  .param pmc img
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "sprite_set_image", "vpp"
  func(item, img)
.end

.sub 'SPR_SetPosition'
  .param pmc item
  .param int x
  .param int y
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "sprite_set_position", "vpii"
  func(item, x, y)
.end

.sub 'SPR_SetCenter'
  .param pmc item
  .param int x
  .param int y
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "sprite_set_center", "vpii"
  func(item, x, y)
.end


.sub 'SPR_GetRotation'
  .param pmc item
  .local pmc lib, func
  .local num retVal
  lib = INT_GetDLL()
  func = dlfunc lib, "sprite_get_rotation", "fp"
  retVal = func(item)
  .return(retVal)
.end


.sub 'SPR_SetRotation'
  .param pmc item
  .param num angle
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "sprite_set_rotation", "vpf"
  func(item, angle)
.end


.sub 'SPR_GetColor'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
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

  lib = INT_GetDLL()
  func = dlfunc lib, "sprite_set_color", "vpp"
  func(item, $P0)
.end


.sub 'POLY_GetPointColor'
  .param pmc item
  .param int index
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "poly_get_point_color", "ppi"
  $P0 = func(item, index)

  #Test
  $P1  = new 'Color'
  $P1.'set_ptr'($P0)
  #Subclass isn't working...

  .return($P1)
.end

.sub 'POLY_SetPointColor'
  .param pmc item
  .param int index
  .param pmc color
  .local pmc lib, func
  $P0 = color.'get_ptr'()

  lib = INT_GetDLL()
  func = dlfunc lib, "poly_set_point_color", "vpip"
  func(item, index, $P0)
.end

.sub 'POLY_SetScaleX'
  .param pmc item
  .param num scale
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "poly_set_scale_x", "vpf"
  func(item, scale)
.end

.sub 'POLY_SetScaleY'
  .param pmc item
  .param num scale
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "poly_set_scale_y", "vpf"
  func(item, scale)
.end

.sub 'POLY_SetPosX'
  .param pmc item
  .param int pos
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "poly_set_pos_x", "vpi"
  func(item, pos)
.end

.sub 'POLY_SetPosY'
  .param pmc item
  .param int pos
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "poly_set_pos_y", "vpi"
  func(item, pos)
.end


.sub 'CLR_SetRed'
  .param pmc item
  .param pmc red
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_red", "vpi"
  func(item, red)
.end


.sub 'CLR_SetGreen'
  .param pmc item
  .param pmc green
  .local pmc lib, func
  $P0 = item.'get_ptr'()

  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_green", "vpi"
  func($P0, green)
.end

.sub 'CLR_SetBlue'
  .param pmc item
  .param pmc blue
  .local pmc lib, func
  $P0 = item.'get_ptr'()

  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_blue", "vpi"
  func($P0, blue)
.end


.sub 'CLR_SetAlpha'
  .param pmc item
  .param pmc alpha
  .local pmc lib, func
  $P0 = item.'get_ptr'()

  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_alpha", "vpi"
  func($P0, alpha)
.end


#Generic delete/draw stuff
.sub 'GAME_DrawItem'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "game_draw_item", "vp"
  func(item)
.end


.sub 'GAME_DeleteItem'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "game_del_item", "vp"
  func(item)
.end

.sub 'GAME_DeleteColor'
  .param pmc item
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "game_del_color", "vp"
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




