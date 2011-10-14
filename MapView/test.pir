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
.end

.sub 'update' :method  
  #Retrieve mouse x,y
  $I0 = INPUT_GetMouseX()
  $I1 = INPUT_GetMouseY()

  #Set polygon's position correctly (proves that Parrot is driving the game)
  DEMO_SetPolyPos($I0 ,$I1)
.end

.sub 'display' :method
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
  .local pmc lib, func
  lib = INT_GetDLL()
  func = dlfunc lib, "init_sfml", "i"
  $I0 = func()
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




#####################################################################
# Main game loop. Makes use of shared library code where possible.
##################################################################### 

.namespace[]
.sub 'main' :main
    $I0 = GAME_Init()
    if $I0==0 goto done

    #Create a sample game object
    $P0 = newclass 'Rendition'
    $P2 = subclass 'Rendition', 'DemoRendition'
    $P1 = new ['DemoRendition']

    say "Starting main loop"

    #Main Loop
    main_loop:
      #Have our game handle expected events
      $I0 = GAME_ProcessEvents()

      #Now update within the game loop
      DEMO_SampleUpdate()

      #Call our sample game object's update method
      $P1.'update'()

      #Display what we've just rendered
      #TODO: This should still exist, but it should do a lot less.
      DEMO_SampleDisplay()

      #And here is where the real updating happens
      $P1.'display'()

      #Continue to update
      if $I0==0 goto main_loop

    #When we're done, clean up
    GAME_Close()

    done:
      say "Done"
.end


