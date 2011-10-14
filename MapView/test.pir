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
  
  #Make the rest of our objects
  makeobjs:

.end

.sub 'update' :method 
  .local pmc drawables 
  .local int index, size

  #Retrieve mouse x,y
  $I0 = INPUT_GetMouseX()
  $I1 = INPUT_GetMouseY()

  #Set polygon's position correctly (proves that Parrot is driving the game)
  DEMO_SetPolyPos($I0 ,$I1)

  #Now, update any objects in our 'drawables' 
  drawables = getattribute self, 'drawables'
  index = 0
  size = drawables

  at_item:
    if index >= size goto done
    $P0 = drawables[index]
    DEMO_UpdatePFXColor($P0)
    index += 1
    goto at_item
  done:
.end

.sub 'display' :method
  .local pmc drawables 
  .local int index, size

  #Now, update any objects in our 'drawables' 
  drawables = getattribute self, 'drawables'
  index = 0
  size = drawables

  at_item:
    if index >= size goto done
    $P0 = drawables[index]
    GAME_DrawItem($P0)
    index += 1
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

#  say 'type of return value: '
#  $S0 = typeof $P0
#  say $S0

#  say 'as pointer: '
#  $I0 = $P0.'pointer'()
#  say $I0


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

    #Assign an attribute to the subclass
    addattribute $P2, 'drawables'

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


