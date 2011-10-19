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



#####################################################################
# Sample rendition subclass that loads our existing demo and displays it.
#####################################################################

.namespace ['DemoRendition']
.sub 'init' :vtable
  .local pmc personImg
  .local int w, h

  #Initialize our polyScale property
  $P0 = new 'Float'
  $N0 = 1.0
  $P0 = $N0
  setattribute self, 'polyScale', $P0

  #Initialize our drawables array
  $P0 = new 'ResizablePMCArray'
  setattribute self, 'drawables', $P0

  #Now, start assigning objects to that array.
  $I0 = PFX_CanUse()
  unless $I0 goto makeobjs

  #Load the shader
  $P1 = PFX_MakeNew('colorize.sfx')
  if null $P1 goto makeobjs

  #Set its properties (for now just use the defaults)
  DEMO_SetDefaultPFX($P1)

  #Save it in our array of objects
  $P0.'push'($P1)
  setattribute self, 'pfxShader', $P1
  
  #Make the rest of our objects
  makeobjs:

  #Images
  personImg = IMG_MakeNew('person.png')
  unless null personImg goto makeobjs2

  #Error
  say "Can't load image: person.png"
  $P1 = new 'Exception'
  throw $P1

  #Make more objects
  makeobjs2:

  #Retrieve the image's width/height, for later
  w = IMG_GetWidth(personImg)
  w /= 2
  h = IMG_GetHeight(personImg)
  h /= 2

  #Make sprite 1, set its properties, save it.
  $P3 = SPR_MakeNew()
  SPR_SetImage($P3, personImg)
  $I0 = 800/3
  $I1 = 600/2
  SPR_SetPosition($P3, $I0, $I1)
  SPR_SetCenter($P3, w, h)
  $P0.'push'($P3)
  setattribute self, 'spr1', $P3


  #Make sprite 2, set its properties, save it.
  $P2 = SPR_MakeNew()
  SPR_SetImage($P2, personImg)
  $I0 = 2*800
  $I0 /= 3
  $I1 = 600/2
  SPR_SetPosition($P2, $I0, $I1)
  SPR_SetCenter($P2, w, h)
  $P0.'push'($P2)
  setattribute self, 'spr2', $P2

.end

.sub 'update' :method 
  .local pmc drawables 
  .local int index, size
  .local num frameTimeS, polyScale

  #Retrieve game timer, current scale
  frameTimeS = GAME_GetFrameTimeInS()
  $P0 = getattribute self, 'polyScale'
  polyScale = $P0

  #Retrieve mouse x,y
  $I0 = INPUT_GetMouseX()
  $I1 = INPUT_GetMouseY()

  #Set polygon's position correctly (proves that Parrot is driving the game)
  DEMO_SetPolyPos($I0 ,$I1)

  #Update our post-effect shader
  $P0 = getattribute self, 'pfxShader'   #Note: This _could_ be null...
  DEMO_UpdatePFXColor($P0)

  #Update sprite 1's rotation
  $P1 = getattribute self, 'spr1'
  $N0 = 50 * frameTimeS
  $N1 = SPR_GetRotation($P1)
  $N0 += $N1
  SPR_SetRotation($P1, $N0)

  #Update sprite 1's colorization
  $P0 = SPR_GetColor($P1)
  $N0 = 1.0-polyScale
  $N0 *= 255
  $I0 = $N0
  CLR_SetBlue($P0, $I0)
  SPR_SetColor($P1, $P0)
  GAME_DeleteColor($P0)

  #Update sprite 2's rotation
  $P1 = getattribute self, 'spr2'
  $N0 = 80 * frameTimeS
  $N1 = SPR_GetRotation($P1)
  $N0 = $N1 - $N0
  SPR_SetRotation($P1, $N0)

  #Update sprite 2's colorization
  $P0 = SPR_GetColor($P1)
  $N0 = polyScale
  $N0 *= 255
  $I0 = $N0
  CLR_SetAlpha($P0, $I0)
  SPR_SetColor($P1, $P0)
  GAME_DeleteColor($P0)

  #Update our polygon's scale factor
  #polyScale += (polyScaleDec?-1:1)*myWindow.GetFrameTime();
  
.end

.sub 'display' :method
  .local pmc drawables 
  .local int index, size

  #Now, update any objects in our 'drawables' 
  drawables = getattribute self, 'drawables'
  index = drawables

  at_item:
    index -= 1
    if index < 0 goto done
    $P0 = drawables[index]
    GAME_DrawItem($P0)
    goto at_item
  done:
.end




#####################################################################
# Shared library functions. Generally PIR need maintain nothing more 
#   than a function pointer; no struct wrapping is required. 
#####################################################################

.namespace[]

#Internal function: Get the DLL
.sub 'INT_GetDLL'
  get_hll_global $P0, 'library_pmc'
  unless null $P0 goto end

  $P0 = loadlib "/home/sethhetu/dubious/MapView/libsfml_engine.so"
  set_hll_global 'library_pmc', $P0
  if $P0 goto end
  
  say "  Couldn't find library!"

  end:
  .return($P0)
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


#Sample internal update function. To be removed.
.sub 'DEMO_SampleUpdate'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "my_basic_update", "v"
  func()
.end


#Sample code to draw objects from SampleUpdate. To be removed.
.sub 'DEMO_SampleDisplay'
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "demo_display", "v"
  func()
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
  .return($P0)
.end


.sub 'SPR_SetColor'
  .param pmc item
  .param pmc color
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "sprite_set_color", "vpp"
  func(item, color)
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
  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_green", "vpi"
  func(item, green)
.end

.sub 'CLR_SetBlue'
  .param pmc item
  .param pmc blue
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_blue", "vpi"
  func(item, blue)
.end


.sub 'CLR_SetAlpha'
  .param pmc item
  .param pmc alpha
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "color_set_alpha", "vpi"
  func(item, alpha)
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
.sub 'main' :main
    .local int res
    .local pmc currRend
    res = GAME_Init(800, 600, 32)
    if res==0 goto done

    #Create Rendition and subclasses
    $P0 = newclass 'Rendition'
    $P2 = newclass 'DemoRendition'
    addparent $P0, $P2

    #Assign some attributes
    addattribute $P2, 'drawables'
    addattribute $P2, 'pfxShader'
    addattribute $P2, 'spr1'
    addattribute $P2, 'spr2'
    addattribute $P2, 'polyScale'

    #Create an object of the subclass.
    currRend = new ['DemoRendition']

    say "Starting main loop"

    #Main Loop
    main_loop:
      #Have our game handle expected events
      $I0 = GAME_ProcessEvents()

      #Now update within the game loop
      DEMO_SampleUpdate()

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


