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

  #Make the shader
  $P1 = PFX_MakeNew('colorize.sfx')
  
  #Make the rest of our objects
  makeobjs:

.end

.sub 'update' :method  
  #Retrieve mouse x,y
  $I0 = INPUT_GetMouseX()
  $I1 = INPUT_GetMouseY()

  #Set polygon's position correctly (proves that Parrot is driving the game)
  DEMO_SetPolyPos($I0 ,$I1)
.end

.sub 'display' :method
  #Draw all drawables
  $P0 = getattribute self, 'drawables'
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
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "new_postfx", "PP"
  $P0 = func(filename)

  say 'type of return value: '
  $S0 = typeof $P0
  say $S0

  .return($P0)
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
      #TODO: This should still exist, but it should do a lot less.
      DEMO_SampleDisplay()

      #And here is where the real updating happens
      currRend.'display'()

      #Continue to update
      if $I0==0 goto main_loop

    #When we're done, clean up
    GAME_Close()

    done:
      say "Done"
.end


